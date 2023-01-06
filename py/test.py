import unittest
from datetime import datetime
# import main as m

test_time = datetime(2022, 12, 25, 0, 6)

class TestSolarCalculations(unittest.TestCase):
    # def test_julian_day(self):
    #     result = m.calc_julian_day(test_time)
    #     expected = 2459938.50  # from external calculator
    #     self.assertEqual(result, expected, "Julian day calculated wrong")

    # def test_julian_century(self):
    #     result = m.calc_julian_year(test_time)
    #     expected = 0.22980162
    #     self.assertAlmostEqual(result, expected)

if __name__ == '__main__':
    res = unittest.main(verbosity=3, exit=False)