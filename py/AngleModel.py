from __future__ import annotations
from datetime import datetime, timezone

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

    def calc_raw_angles(t):
        """
        Calculate the solar angles for a given time in global frame
        """
        raise NotImplementedError()

    def calc_adjusted_angles(t):
        """
        Calculate the solar angles for a given time in local frame (adjusted by offset)
        """
        raise NotImplementedError()

if __name__ == "__main__":
    print("Yes")
    a = Angle(40)
    c = SolarAngles(20, 20)
    print(c)
    dt = datetime.now(timezone.utc)
    print(dt.date())
    print(dt.time())


    # print()