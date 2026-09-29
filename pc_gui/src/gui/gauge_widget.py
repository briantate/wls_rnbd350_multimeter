"""Analog gauge widget for displaying resistance."""

import math
import tkinter as tk
from typing import Optional

import customtkinter as ctk


class GaugeWidget(ctk.CTkFrame):
    """Semicircular analog meter gauge with animated needle."""

    NEEDLE_SWEEP_DEGREES = 180
    ANIMATION_STEP_MS = 16
    ANIMATION_SMOOTHING = 0.15

    BG_COLOR = "#1a1a2e"
    ARC_COLOR = "#2d2d44"
    TICK_COLOR = "#e0e0e0"
    NEEDLE_COLOR = "#ff4444"
    TEXT_COLOR = "#ffffff"
    SCALE_COLOR = "#cccccc"
    OVER_RANGE_COLOR = "#ff6600"
    DIGITAL_BG = "#0a0a14"

    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)

        self._current_value = 0.0
        self._target_value = 0.0
        self._range_label = "1 kΩ"
        self._digital_value = "0 Ω"
        self._over_range = False
        self._animating = False

        self.configure(fg_color=self.BG_COLOR)

        self._canvas = tk.Canvas(
            self,
            bg=self.BG_COLOR,
            highlightthickness=0,
        )
        self._canvas.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self._digital_frame = ctk.CTkFrame(self, fg_color=self.DIGITAL_BG, corner_radius=8)
        self._digital_frame.pack(fill=tk.X, padx=20, pady=(0, 15))

        self._digital_label = ctk.CTkLabel(
            self._digital_frame,
            text="0 Ω",
            font=ctk.CTkFont(family="Consolas", size=48, weight="bold"),
            text_color=self.TEXT_COLOR,
        )
        self._digital_label.pack(pady=15)

        self._over_range_label = ctk.CTkLabel(
            self._digital_frame,
            text="",
            font=ctk.CTkFont(size=18, weight="bold"),
            text_color=self.OVER_RANGE_COLOR,
        )
        self._over_range_label.pack(pady=(0, 5))

        self._canvas.bind("<Configure>", self._on_resize)

    def _on_resize(self, event: Optional[tk.Event] = None) -> None:
        """Handle canvas resize."""
        self._draw_gauge()

    def _draw_gauge(self) -> None:
        """Draw the complete gauge."""
        self._canvas.delete("all")

        width = self._canvas.winfo_width()
        height = self._canvas.winfo_height()

        if width < 50 or height < 50:
            return

        padding = 30
        gauge_height = height - 40
        radius = min((width - 2 * padding) / 2, gauge_height - 20)

        center_x = width / 2
        center_y = height - 40

        self._draw_arc_background(center_x, center_y, radius)
        self._draw_scale(center_x, center_y, radius)
        self._draw_needle(center_x, center_y, radius * 0.85)
        self._draw_center_pivot(center_x, center_y)
        self._draw_range_label(center_x, center_y, radius)

    def _draw_arc_background(self, cx: float, cy: float, radius: float) -> None:
        """Draw the gauge background arc."""
        self._canvas.create_arc(
            cx - radius,
            cy - radius,
            cx + radius,
            cy + radius,
            start=0,
            extent=180,
            fill=self.ARC_COLOR,
            outline=self.TICK_COLOR,
            width=2,
            style=tk.PIESLICE,
        )

    def _draw_scale(self, cx: float, cy: float, radius: float) -> None:
        """Draw scale ticks and labels."""
        num_major_ticks = 11
        num_minor_per_major = 4

        for i in range(num_major_ticks):
            angle_deg = 180 - (i * 180 / (num_major_ticks - 1))
            angle_rad = math.radians(angle_deg)

            outer_r = radius * 0.92
            inner_r = radius * 0.78
            label_r = radius * 0.65

            x1 = cx + outer_r * math.cos(angle_rad)
            y1 = cy - outer_r * math.sin(angle_rad)
            x2 = cx + inner_r * math.cos(angle_rad)
            y2 = cy - inner_r * math.sin(angle_rad)

            self._canvas.create_line(
                x1, y1, x2, y2, fill=self.TICK_COLOR, width=3
            )

            label_x = cx + label_r * math.cos(angle_rad)
            label_y = cy - label_r * math.sin(angle_rad)

            value = i * 10
            font_size = max(10, int(radius / 12))
            self._canvas.create_text(
                label_x,
                label_y,
                text=str(value),
                fill=self.SCALE_COLOR,
                font=("Arial", font_size, "bold"),
            )

            if i < num_major_ticks - 1:
                for j in range(1, num_minor_per_major + 1):
                    minor_angle_deg = angle_deg - (j * 180 / (num_major_ticks - 1) / (num_minor_per_major + 1))
                    minor_angle_rad = math.radians(minor_angle_deg)

                    minor_outer_r = radius * 0.92
                    minor_inner_r = radius * 0.84

                    mx1 = cx + minor_outer_r * math.cos(minor_angle_rad)
                    my1 = cy - minor_outer_r * math.sin(minor_angle_rad)
                    mx2 = cx + minor_inner_r * math.cos(minor_angle_rad)
                    my2 = cy - minor_inner_r * math.sin(minor_angle_rad)

                    self._canvas.create_line(
                        mx1, my1, mx2, my2, fill=self.TICK_COLOR, width=1
                    )

    def _draw_needle(self, cx: float, cy: float, length: float) -> None:
        """Draw the needle at current position."""
        angle_deg = 180 - (self._current_value * 180)
        angle_rad = math.radians(angle_deg)

        needle_tip_x = cx + length * math.cos(angle_rad)
        needle_tip_y = cy - length * math.sin(angle_rad)

        needle_width = max(4, length / 25)
        perp_angle = angle_rad + math.pi / 2

        base_offset = needle_width / 2
        base_x1 = cx + base_offset * math.cos(perp_angle)
        base_y1 = cy - base_offset * math.sin(perp_angle)
        base_x2 = cx - base_offset * math.cos(perp_angle)
        base_y2 = cy + base_offset * math.sin(perp_angle)

        color = self.OVER_RANGE_COLOR if self._over_range else self.NEEDLE_COLOR

        self._canvas.create_polygon(
            needle_tip_x,
            needle_tip_y,
            base_x1,
            base_y1,
            base_x2,
            base_y2,
            fill=color,
            outline=color,
        )

    def _draw_center_pivot(self, cx: float, cy: float) -> None:
        """Draw the center pivot point."""
        pivot_radius = 12
        self._canvas.create_oval(
            cx - pivot_radius,
            cy - pivot_radius,
            cx + pivot_radius,
            cy + pivot_radius,
            fill="#333344",
            outline=self.TICK_COLOR,
            width=2,
        )

    def _draw_range_label(self, cx: float, cy: float, radius: float) -> None:
        """Draw the current range label."""
        font_size = max(12, int(radius / 8))
        self._canvas.create_text(
            cx,
            cy - radius * 0.35,
            text=f"Range: {self._range_label}",
            fill=self.TEXT_COLOR,
            font=("Arial", font_size),
        )

    def set_value(
        self,
        normalized: float,
        digital: str,
        range_label: str,
        over_range: bool = False,
    ) -> None:
        """Set the gauge value with animation."""
        self._target_value = max(0.0, min(1.0, normalized))
        self._digital_value = digital
        self._range_label = range_label
        self._over_range = over_range

        self._digital_label.configure(text=digital)
        self._over_range_label.configure(text="OVER RANGE" if over_range else "")

        if not self._animating:
            self._animating = True
            self._animate()

    def _animate(self) -> None:
        """Animate the needle movement."""
        diff = self._target_value - self._current_value

        if abs(diff) < 0.001:
            self._current_value = self._target_value
            self._animating = False
            self._draw_gauge()
            return

        self._current_value += diff * self.ANIMATION_SMOOTHING
        self._draw_gauge()

        self.after(self.ANIMATION_STEP_MS, self._animate)

    def reset(self) -> None:
        """Reset the gauge to zero."""
        self._current_value = 0.0
        self._target_value = 0.0
        self._range_label = "1 kΩ"
        self._digital_value = "0 Ω"
        self._over_range = False
        self._digital_label.configure(text="0 Ω")
        self._over_range_label.configure(text="")
        self._draw_gauge()
