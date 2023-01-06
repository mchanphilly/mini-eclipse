import numpy as np

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

    def find_position(self, angles, position):
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
        """
        if len(angles) != 2:
            raise ValueError("Angles needs an azimuth and altitude")
        if len(position) != 3:
            raise ValueError("Position needs x, y, z")

        azimuth, altitude = angles
        viewer_x, viewer_y, viewer_z = position

        if viewer_z > 0:
            raise ValueError("z should be negative if inside the room")

        if altitude < 0:
            raise ValueError("altitude is negative; maybe the sun is beneath the horizon")

        # Note the negative because the viewer is inside and we're moving the viewer_z to 0.
        position_x = viewer_x + viewer_z * np.tan(azimuth)

        # Note the double negative left in for clarity. Offset is subtracted here because
        # of how the triangle is set up.
        position_y = viewer_y + viewer_z * np.tan(altitude)
        
        return (position_x, position_y)

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

def main():
    # Units typically in inches
    width, height = 45.5, 57.5
    window = Window(width, height)

    # Approximate position of my eyes when I'm at my desk
    position = (15, height - 9, 35)

    # window.plot_points([1, 2, 3], [4, 5, 6])
    # ax.plot([1, 2, 3, 4], [1, 4, 2, 3]);  # Plot some data on the axes.


if __name__ == "__main__":
    main()