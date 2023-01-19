import matplotlib as mpl
import matplotlib.pyplot as plt

from sun_model import SolarModel, SolarAngles, Location
from window import Window

class VirtualEclipse:
    """
    Represents a virtual implmementation of the Mini-Eclipse project
    """

    def __init__(self, window: Window, solar = SolarModel) -> None:
        self.window = window
        self.solar = solar

    def plot_window(self, ax):
        """
        Creates the "window" figure with the edges of the window.
        """
        w, h = self.window.width, self.window.height

        # Construct the window framing
        corners = [(0, 0), (w, 0), (w, h), (0, h)]
        window_edges = [(corners[i], corners[(i+1)%4]) for i in range(4)]
        lines = mpl.collections.LineCollection(window_edges, color="black")
        
        ax.add_collection(lines)
        ax.invert_yaxis()
        ax.axis("equal")

        return ax

    def plot_all_points(self, ax, xs, ys):
        """
        Given an iterable x and corresponding iterable of y, plot these points.
        """
        ax.scatter(xs, ys)
        return ax

    def plot_all_strings(self, ax, xys):
        """
        Given an iterable of (x, y) points, plot the strings to those points.
        """
        top_left = (0, 0)
        top_right = (self.window.width, 0)

        strings_sets = [[[corner, point] for point in xys] for corner in (top_left, top_right)]

        lines_sets = [mpl.collections.LineCollection(string_set, color="gray") for string_set in strings_sets]
        [ax.add_collection(lines) for lines in lines_sets]
        return ax

    def get_today_points(self, position, steps: int = 2):
        """
        Get today's points as an iterable of (x, y) pairs and times in tuple pair (xys, times)
        
        Sunrise and sunset are guaranteed.

        Each step is equally spaced in time, and the iterable is of length steps.

        Note that the first and last steps aren't guaranteed to be on the plot, so more granularity
        is required if the position is further from the window.
        
        TODO Future development might allow steps to describe points within the bounds of the window,
        though this would require some processing to see how we get those points. (e.g. if we're right 
        on the window, all points of the day qualify as long as we can see sunrise and sunset, but 
        if we're all the way in the back of the room, we might see only a narrow part of the day.)
        TODO See get_included_points()
        """
        times = self.solar.split_today_times(steps)
        angles = self.solar.times_to_angles(times)

        # xys is a list of (x,y) pairs
        xys = [self.window.find_position(angle, position) for angle in angles]

        return (xys, times)

    def get_included_points(self, position, steps: int = 2):
        """
        TODO 
        Get today's points as an iterable of (x, y) pairs, but only the points that are
        within the frame of the window from the position.

        TODO Implementation tips:
        - Could preprocess to see the angle bounds from the position (both azimuthal and altitudinal)
        - Could binary search to see what times we see the early and late bound of each angular coordinate.
        - Could do linear interpolation in time between the two inner times of the four above times.
        - Process from there.
        """
        raise NotImplementedError()

    def plot_all_today(self, position, steps: int = 2):
        """
        Plots today's points on the window, where the number of points is steps.
        """
        xys, _ = self.get_today_points(self, position, steps)

        # xs, ys are two separate lists of the x and y
        xs, ys = list(zip(*xys))
        
        fig, ax = plt.subplots()
        ax = self.plot_window(ax)
        ax = self.plot_points(ax, xs, ys)
        ax = self.plot_strings(ax, xys)

    def plot_today(self, position, steps: int = 2):
        """
        Plots an interactive simulation of a day's worth of movements according to steps.
        """
        xys, times = self.get_included_points(self, position, steps)

        fig, ax = plt.subplots()
        ax = self.plot_window(ax)
        


def main():
    width, height = 45.5, 57.5
    window = Window(width, height)

    test_loc = Location(42.360015, -71.087902)
    test_offset = SolarAngles(155.75, 0)
    solar = SolarModel(test_loc, test_offset)

    eclipse = VirtualEclipse(window, solar)

    known_noon_angles = SolarAngles(-0.367949956722299, 21.17302940256547)
    # Approximate position of my eyes when I'm at my desk
    position = (15, height - 9, 35)

    print(eclipse.plot_today(position, 40))

if __name__ == "__main__":
    main()