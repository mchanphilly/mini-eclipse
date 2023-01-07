import numpy as np
from sun_model import SolarAngles

"""
Module for doing all the window calculations.
"""

class Window:
    """
    time + location -> sun angles
    sun angles + window offset -> window angles
    window angles + viewer position -> window cartesian
    window cartesian + window size -> string lengths
    string lengths + motor parameters -> motor commands
    """
    
    def __init__(self, w, h) -> None:
        self.width = w
        self.height = h

    def find_position(self, angles: SolarAngles, position, override: bool = False):
        """
        Using the window angles and the viewer position, find
        the (x, y) pair that would block the light.

        See documentation for more details, but we're using a left-handed XYZ centered
        around the top left corner, where X points right, Y points down, and Z points in.

        angles is an iterable of length 2 with sun azimuth and altitude relative
        to the window, where:
            - azimuth is
                    -90 when pointing to the negative-x (left),
                    90 when pointing to positive-x (right)
            - altitude is
                    0 when pointing to negative-z (outside)
                    90 when pointing negative-y (up)

        position is an iterable of length 3 with x, y, z offsets from the window's
        origin to the position of the viewer.

        Think of this as projecting a point in the room onto a point in the plane of
        the window.

        If the point is out of the window, return (0,0) unless override
        """
        if len(position) != 3:
            raise ValueError("Position needs x, y, z")

        # A little messy, but we convert here to radians to work with np
        azimuth, altitude = angles.to_radian().get_angles()
        viewer_x, viewer_y, viewer_z = position

        if viewer_z < 0:
            raise ValueError("z is negative but should be positive if inside the room")

        lower_altitude = np.deg2rad(-15)
        if altitude < lower_altitude:
            raise ValueError(f"altitude is too low; below {lower_altitude}")

        # Note the negative because the viewer is inside and we're moving the viewer_z to 0.
        position_x = viewer_x + viewer_z * np.tan(azimuth)

        # Note the double negative left in for clarity. Offset is subtracted here because
        # of how the triangle is set up.
        position_y = viewer_y - viewer_z * np.tan(altitude)
        
        # return (position_x, position_y)
        if override or ((0 < position_x < self.width) and (0 < position_y < self.height)):
            return (position_x, position_y)
        else:
            return (0, 0)

    def cartesian_to_lengths(self, x, y):
        """
        returns tuple (l1, l2) where l1 corresponds to 
        the string connected to the origin and l2 corresponds
        to the string connected to the opposite top corner.
        """
        if x < 0:
            raise ValueError("x needs to be non-negative")
        if y < 0:
            raise ValueError("y needs to be non-negative")
        if x > self.width:
            raise ValueError("x needs to be less than the window's width")
        if y > self.height:
            raise ValueError("y needs to be less than the window's height")

        l_1 = np.square(x) + np.square(y)
        l_2 = np.square(self.width - x) + np.square(y)

        return (l_1, l_2)

def test():
    w, h = 20, 20
    p = (10, 10, 10)
    window = Window(w, h)
    print(window.find_position(SolarAngles(0, 70), p))

def main():
    # Units typically in inches
    real_width, real_height = 45.5, 57.5
    real_window = Window(real_width, real_height)

    # Approximate position of my eyes when I'm at my desk
    real_position = (15, real_height - 9, 35)

    test()

    # window.plot_points([1, 2, 3], [4, 5, 6])
    # ax.plot([1, 2, 3, 4], [1, 4, 2, 3]);  # Plot some data on the axes.


if __name__ == "__main__":
    main()