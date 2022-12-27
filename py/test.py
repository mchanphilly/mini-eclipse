import unittest
from datetime import datetime
import main as m

test_time = datetime(2022, 12, 25, 0, 6)
test_loc = m.Location(42.360015, -71.087902)
test_offset = m.SolarAngles(155.75, 0)
model = m.SolarModel(test_loc, test_offset)

class TestSolarCalculations(unittest.TestCase):
    
    def test_julian_day(self):
        result = m.calc_julian_day(test_time)
        expected = 2459938.50  # from external calculator
        self.assertEqual(result, expected, "Julian day calculated wrong")

    def test_julian_century(self):
        result = m.calc_julian_year(test_time)
        expected = 0.22980162
        self.assertAlmostEqual(result, expected)

    def test_sun_declin(self):
        expected = -23.40049995

    def test_hour_angle(self):
        expected = 110.4599971

    def test_zenith_angle(self):
        result = model._calc_zenith()
        expected = 120.3079889
        self.assertAlmostEqual(result, expected)

if __name__ == '__main__':
    res = unittest.main(verbosity=3, exit=False)