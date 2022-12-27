import unittest, datetime
import AngleModel

class TestSolarCalculations(unittest.TestCase):
    test_time = datetime(2022, 12, 25, 0, 6)
    test_loc = Location(42.360015, -71.087902)
    test_offset = SolarAngles(155.75, 0)
    model = SolarModel(test_loc, test_offset)

    print(model.calc_raw_angles(test_time))
    def test_julian_day(self):
        actual = model.calc_raw_angles(test_time)
        expected = 2459938.50
        self.assertEqual(actual, expected, "Julian day calculated wrong")

if __name__ == '__main__':
    runner = unittest.TextTestRunner()
    runner.run(TestSolarCalculations())