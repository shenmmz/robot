from __future__ import annotations

import unittest

import numpy as np

from app.models import Point3D, Side
from solver.wrist import PalmRollTracker


class PalmRollTrackerTests(unittest.TestCase):
    @staticmethod
    def _hand_points(angle_deg: float) -> list[Point3D]:
        base = np.array(
            [
                [-0.040, 0.0, 0.100],
                [-0.015, 0.0, 0.120],
                [0.015, 0.0, 0.115],
                [0.040, 0.0, 0.100],
            ],
            dtype=np.float64,
        )
        angle = np.radians(angle_deg)
        rotation = np.array(
            [
                [np.cos(angle), np.sin(angle), 0.0],
                [-np.sin(angle), np.cos(angle), 0.0],
                [0.0, 0.0, 1.0],
            ],
            dtype=np.float64,
        )
        rotated = base @ rotation
        return [Point3D(0.0, 0.0, 0.0), *[Point3D(*point) for point in rotated]]

    @staticmethod
    def _update(tracker: PalmRollTracker, angle_deg: float):
        points = PalmRollTrackerTests._hand_points(angle_deg)
        return tracker.update(
            hand_wrist=points[0],
            index_mcp=points[1],
            middle_mcp=points[2],
            ring_mcp=points[3],
            pinky_mcp=points[4],
            world_up=np.array([0.0, 1.0, 0.0]),
            detection_confidence=1.0,
        )

    def _tracker(self) -> PalmRollTracker:
        return PalmRollTracker(
            Side.RIGHT,
            smoothing_alpha=1.0,
            max_step_deg=180.0,
            min_confidence=0.1,
        )

    def test_signed_roll_distinguishes_left_and_right(self) -> None:
        positive = self._tracker()
        self._update(positive, 0.0)
        self.assertAlmostEqual(self._update(positive, 60.0).signed_roll_deg or 0.0, 60.0, places=5)

        negative = self._tracker()
        self._update(negative, 0.0)
        self.assertAlmostEqual(self._update(negative, -60.0).signed_roll_deg or 0.0, -60.0, places=5)

    def test_roll_is_clamped_without_90_degree_foldback(self) -> None:
        tracker = self._tracker()
        values = [self._update(tracker, angle).signed_roll_deg for angle in (0.0, 89.0, 91.0, 120.0)]
        self.assertAlmostEqual(values[1] or 0.0, 89.0, places=5)
        self.assertAlmostEqual(values[2] or 0.0, 90.0, places=5)
        self.assertAlmostEqual(values[3] or 0.0, 90.0, places=5)

    def test_missing_hand_does_not_emit_motion(self) -> None:
        tracker = self._tracker()
        tracked = self._update(tracker, 30.0)
        missing = tracker.missing("hands_missing")
        self.assertTrue(tracked.valid)
        self.assertFalse(missing.valid)
        self.assertFalse(missing.motion_allowed)
        self.assertAlmostEqual(missing.signed_roll_deg or 0.0, tracked.signed_roll_deg or 0.0)


if __name__ == "__main__":
    unittest.main()
