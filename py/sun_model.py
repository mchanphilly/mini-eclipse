from __future__ import annotations
from datetime import datetime, timezone, timedelta
from suncalc import get_position, get_times
from numpy import pi
from enum import Enum

class Location():
    def __init__(self, latitude, longitude) -> None:
        self.latitude = latitude
        self.longitude = longitude
        self.unit = AngleUnit.DEGREE

    def get_lat_lon(self):
        return (self.latitude, self.longitude)

class AngleUnit(Enum):
    RADIAN = 1
    DEGREE = 2

class SolarAngles():

    def __init__(self, azimuth, altitude, unit=AngleUnit.DEGREE) -> None:
        self.angles = (azimuth, altitude)
        self.unit = unit

    def to_degree(self) -> SolarAngles:
        """
        Assumes it was currently in radians and returns a new SolarAngles 
        object in degrees
        """
        if self.unit != AngleUnit.RADIAN:
            raise TypeError("These angles aren't in radians.")

        angles = (a * 180 / pi for a in self.angles)
        return SolarAngles(*angles, unit=AngleUnit.DEGREE)

    def __sub__(self, other: SolarAngles) -> SolarAngles:
        if (self.unit != other.unit):
            raise TypeError(f"Different AngleUnit: this is in {self.unit} but the other is {other.unit}")

        out_angles = [self.angles[i] - other.angles[i] for i in range(2)]
        return SolarAngles(*out_angles, unit=self.unit)

    def __repr__(self) -> str:                
        return f"SolarAngles{self.angles}" + (self.unit.name if self.unit == AngleUnit.RADIAN else "")

class SolarModel:
    """
    Calculates the position of the sun given a certain pose (position and orientation)
    """
    
    location: Location = None
    offset: SolarAngles = None

    def __init__(self, loc: Location, os: SolarAngles) -> None:
        """
        Location is latitude, longitude
        Offset is what you would add to north to get your window positioning
        """
        self.location = loc
        self.offset = os

    def calc_raw_angles(self, t: datetime, unit=AngleUnit.DEGREE) -> SolarAngles:
        """
        Calculate the solar angles for a given UTC time in global frame
        """
        lat, lon = self.location.get_lat_lon()
        position = get_position(t, lon, lat)
        
        angles = SolarAngles(pi + position["azimuth"], position["altitude"], unit=AngleUnit.RADIAN)

        if unit == AngleUnit.DEGREE:
            angles = angles.to_degree()

        return angles

    def calc_adjusted_angles(self, t: datetime, unit=AngleUnit.DEGREE) -> SolarAngles:
        """
        Calculate the solar angles for a given time in local frame (adjusted by offset)
        """
        raw_angles = self.calc_raw_angles(t, unit)
        return raw_angles - self.offset

    def calc_ends(self, day: datetime) -> tuple[datetime, datetime]:
        """
        Returns sunrise and sunset, AKA when the mini-eclipse should be active.

        day should be any datetime on the day we're looking at.
        """
        lat, lon = self.location.get_lat_lon()
        times = get_times(day, lon, lat)
        out = times["sunrise"], times["sunset_start"]

        # These are returned in UTC but they don't have UTC info yet,
        # so we add it in
        return [d.replace(tzinfo=timezone.utc) for d in out]

    def split_day_times(self, day: datetime, steps: int = 2):
        """
        Returns a list of datetime of length 'steps' to split the day.
        
        steps must always be at least 2, and the output always starts with
        sunrise and ends with sunset.
        """
        if steps < 2:
            raise ValueError("Must at least be 2 steps")

        sunrise, sunset = self.calc_ends(day)
        total = sunset - sunrise

        step_amount = total / (steps - 1)
        out = [sunrise + i * step_amount for i in range(steps)]
        return out

    def split_today_times(self, steps: int = 2):
        """
        Returns split_day_times for today
        """
        today = datetime.utcnow()
        # print(f"Today is {today.date()}")
        return self.split_day_times(today, steps)

    def times_to_angles(self, times):
        """
        Return a list of SolarAngles corresponding to a list of times.
        """
        return [self.calc_adjusted_angles(t) for t in times]

    @staticmethod
    def readable_times(times: list, verbose: bool = False):
        """
        Turns a list of datetime into a list of readable strings. Offers
        a more compact version if the times are on the same date.
        """
        def all_same(items: list, f: function):
            return all([f(item) == f(items[0]) for item in items])

        same_date = all_same(times, lambda dt: dt.date())
        format_string = ("%b %d" if not same_date or verbose else "") + "%H:%M"

        return [time.strftime(format_string) for time in times]

def main():
    # test_time = datetime(2022, 12, 25, 0, 6)

    # The location I'm testing from
    test_loc = Location(42.360015, -71.087902)
    test_offset = SolarAngles(155.75, 0)
    model = SolarModel(test_loc, test_offset)

    utc_now = datetime.utcnow()    
    # test_time = datetime(2022, 12, 27, 15, 10, tzinfo=timezone.utc)
    zero_azimuth_time = datetime(2023, 1, 6, 15, 11, tzinfo=timezone.utc)
    sunset_time = datetime(2023, 1, 6, 21, 27, tzinfo=timezone.utc)

    def print_test(t: datetime, s: str):
        position = model.calc_adjusted_angles(t)
        raw_position = model.calc_raw_angles(t)
        splits = model.split_day_times(t, 3)
        # print(s, position)
        print(SolarModel.readable_times(splits))
        # print([model.calc_raw_angles(t) for t in splits])
        # print(model.calc_adjusted_angles(sunset_time))
        # print(end_times)
        # print(sunset_time - splits[-1])
        # print(s, raw_position)

    # print_test(zero_azimuth_time, "zero azimuth")
    # print_test(sunset_time, "sunset")

    # print(testing(utc_now))
    # print(testing(test_time))

    # times = model.split_today(5)
    # angles = [model.calc_adjusted_angles(time) for time in times]
    
    # print(times)
    # print(angles)


if __name__ == "__main__":
    main()