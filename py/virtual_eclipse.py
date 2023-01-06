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
        w, h = self.width, self.height

        # Construct the window framing
        corners = [(0, 0), (w, 0), (w, h), (0, h)]
        window_edges = [(corners[i], corners[(i+1)%4]) for i in range(4)]
        lines = mpl.collections.LineCollection(window_edges, color="black")
        ax.add_collection(lines)

        # ax.scatter([1, 2, 3], [4, 5, 6])
        ax.scatter(xs, ys)
        ax.invert_yaxis()
        ax.axis("equal")

def main():
    width, height = 45.5, 57.5
    window = Window(width, height)

    test_loc = Location(42.360015, -71.087902)
    test_offset = SolarAngles(155.75, 0)
    solar = SolarModel(test_loc, test_offset)

    eclipse = VirtualEclipse(window, solar)

    # Approximate position of my eyes when I'm at my desk
    position = (15, height - 9, 35)

if __name__ == "__main__":
    main()