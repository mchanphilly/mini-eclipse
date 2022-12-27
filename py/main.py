from __future__ import annotations
from datetime import datetime, timezone, timedelta

class Angle:
    value: int = None
    
    def __init__(self, v: int) -> None:
        self.value = v

    def __add__(self, v: Angle) -> Angle:
        self.value + v.value

    def __repr__(self) -> str:
        return f"Angle({self.value})"

class AngularPair:
    angles: tuple(Angle, Angle) = None

    def __init__(self, a1: Angle, a2: Angle) -> None:
        self.angles = (a1, a2)

    def __add__(self, ap: AngularPair) -> AngularPair:
        return tuple(self.angles[i] + ap[i] for i in range(2))

    def __getitem__(self, i: int) -> Angle:
        if i >= 2:
            raise IndexError()
        return self.angles[i]

    def __repr__(self) -> str:
        return f"<{self.angles[0]}, {self.angles[1]}>"


class Location(AngularPair):
    """
    Represents a coordinate location on Earth.
    """
    def __repr__(self) -> str:
        return "Location" + super().__repr__()

class SolarAngles(AngularPair):
    """
    Represents a sun position with azimuth and altitude.
    """
    def __repr__(self) -> str:
        return "SolarAngles" + super().__repr__()

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

    def calc_raw_angles(self, t: datetime) -> SolarAngles:
        """
        Calculate the solar angles for a given UTC time in global frame
        """
        raise NotImplementedError

    def calc_adjusted_angles(self, t: datetime):
        """
        Calculate the solar angles for a given time in local frame (adjusted by offset)
        """
        raise NotImplementedError()

def calc_julian_day(t:datetime) -> float:
    JULIAN_CONSTANT = 2415018.5
    julian_epoch = datetime(1900, 1, 1)
    since = t - julian_epoch
    julian_day = JULIAN_CONSTANT + since.days + 2
    
    return julian_day

if __name__ == "__main__":
    print("Yes")
    a = Angle(40)
    c = SolarAngles(20, 20)
    # print(c)
    dt = datetime.now(timezone.utc)
    # print(dt.date())
    # print(dt.time())

    # test_time = datetime(2010, 6, 21, 0, 6)
    # print(test_time)

    # home = Location(42.360015, -71.087902)
    # offset = SolarAngles(155.75, 0)
    # model = SolarModel(home, offset)

    # print(model.calc_raw_angles(test_time))