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

    def plot_points(self, xs, ys):
        """
        Given an iterable of (x, y) points, plot them.
        """
        fig, ax = plt.subplots()
        w, h = self.window.width, self.window.height

        # Construct the window framing
        corners = [(0, 0), (w, 0), (w, h), (0, h)]
        window_edges = [(corners[i], corners[(i+1)%4]) for i in range(4)]
        lines = mpl.collections.LineCollection(window_edges, color="black")
        ax.add_collection(lines)

        # ax.scatter([1, 2, 3], [4, 5, 6])
        ax.scatter(xs, ys)
        ax.invert_yaxis()
        ax.axis("equal")

    def plot_today(self, position, steps: int = 2):
        """
        Plots today's points on the window, where the number of points is steps.
        """
        times = self.solar.split_today_times(steps)
        angles = self.solar.times_to_angles(times)
        # print(angles)
        xys = [self.window.find_position(angle, position) for angle in angles]
        # print(xys)
        xs, ys = list(zip(*xys))
        # print(xs)
        self.plot_points(xs, ys)



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