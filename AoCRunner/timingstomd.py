import json
import sys

HEADERS = [
    "Year",
    "Day",
    "Part",
    "PC (ms)",
    "Ratio",
    "RP2040 (ms)",
    "RP2350 (ms)",
    "RP2350 @ 300MHz (ms)",
]

GREEN = "\U0001F7E2"  # green circle
AMBER = "\U0001F7E1"  # yellow circle
RED = "\U0001F534"    # red circle

def time_marker_pc(value):
    if value < 7:
        return GREEN
    if value < 15:
        return AMBER
    return RED

def time_marker_pico(value):
    if value < 900:
        return GREEN
    if value < 1500:
        return AMBER
    return RED

def fmt_pc_time(value):
    if value is None:
        return "-"
    return f"{time_marker_pc(value)} {value:,.3f}"

def fmt_pico_time(value):
    if value is None:
        return "-"
    return f"{time_marker_pico(value)} {value:,.1f}"

def ratio_marker(ratio, rp2040):
    if rp2040 < 100:
        return GREEN
    r = round(ratio, 1)
    if r < 250:
        return GREEN
    if r < 500:
        return AMBER
    return RED

def render_row(year, day, part, t):
    pc = t.get("PC")
    rp2040 = t.get("RP2040")

    rp2040_txt = fmt_pico_time(rp2040)

    if rp2040 is not None and pc:
        ratio = rp2040 / pc
        ratio_txt = f"{ratio_marker(ratio, rp2040)} {ratio:,.1f}"
    else:
        ratio_txt = "-"

    cells = [
        year,
        day,
        part,
        fmt_pc_time(pc),
        ratio_txt,
        rp2040_txt,
        fmt_pico_time(t.get("RP2350")),
        fmt_pico_time(t.get("RP2350_300MHz")),
    ]
    return "| " + " | ".join(cells) + " |"

def render_year_table(year, days):
    header = "| " + " | ".join(HEADERS) + " |"
    divider = "| " + " | ".join("---" for _ in HEADERS) + " |"

    body = []
    for day in sorted(days, key=int):
        parts = days[day]
        for part in sorted(parts, key=int):
            body.append(render_row(year, day, part, parts[part]))

    return f"## {year}\n\n" + "\n".join([header, divider, *body]) + "\n"


with open("timings.json", encoding="utf-8") as f:
    data = json.load(f)

sections = ["# Solution times"]
for year in sorted(data, key=int):
    sections.append(render_year_table(year, data[year]))

with open("TIMINGS.md", "w", encoding="utf-8") as f:
    f.write("\n".join(sections))
