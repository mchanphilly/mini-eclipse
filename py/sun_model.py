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
        Assumes it was currently in radians.
        """
        if self.unit != AngleUnit.RADIAN:
            raise TypeError("These angles aren't in radians.")

        angles = (a * 180 / pi for a in self.angles)
        self.unit = AngleUnit.DEGREE
        return SolarAngles(*angles)

    def __sub__(self, other: SolarAngles) -> SolarAngles:
        if (self.unit != other.unit):
            raise TypeError(f"Different AngleUnit: this is in {self.unit} but the other is {other.unit}")

        angles = [self.angles[i] - other.angles[i] for i in range(2)]
        return SolarAngles(*angles, unit=self.unit)

    def __repr__(self) -> str:                
        return f"SolarAngles{self.angles} {self.unit}"

class SolarModel:
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

    def calc_ends(self, t: datetime) -> tuple[datetime, datetime]:
        """
        Returns sunrise and sunset, AKA when the mini-eclipse should be active.
        """
        lat, lon = self.location.get_lat_lon()
        times = get_times(t, lon, lat)
        return times["sunrise"], times["sunset_start"]

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
        end_times = [end.strftime("%b %d %H:%M:%S") for end in model.calc_ends(t)]
        print(s, position)
        # print(end_times)
        # print(s, raw_position)

    print_test(zero_azimuth_time, "zero azimuth")
    print_test(sunset_time, "sunset")

    # print(testing(utc_now))
    # print(testing(test_time))



if __name__ == "__main__":
    main()