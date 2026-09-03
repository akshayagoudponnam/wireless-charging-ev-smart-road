from flask import Flask, render_template_string
import csv
from pathlib import Path

app = Flask(__name__)
DATA_FILE = Path(__file__).with_name("DATA.csv")

HTML = """
<!doctype html>
<html>
<head>
  <meta charset="utf-8">
  <title>WPT EV Charging Dashboard</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 40px; background: #f5f7fa; }
    .card { background: white; padding: 20px; margin: 12px 0; border-radius: 10px; }
    h1 { margin-bottom: 6px; }
    table { border-collapse: collapse; width: 100%; background: white; }
    th, td { border: 1px solid #ddd; padding: 10px; text-align: left; }
    th { background: #eee; }
    .note { color: #555; }
  </style>
</head>
<body>
  <h1>Wireless Power Transfer – EV Monitoring</h1>
  <p class="note">Companion local dashboard for the project repository. The supplied report describes ThingSpeak as the original cloud-monitoring platform.</p>
  <div class="card">
    <h2>Data source</h2>
    <p>{{ filename }}</p>
    <p>{{ count }} observation(s) available in DATA.csv.</p>
  </div>
  <div class="card">
    <h2>Latest available observation</h2>
    {% if latest %}
    <table>
      <tr>{% for k in latest.keys() %}<th>{{ k }}</th>{% endfor %}</tr>
      <tr>{% for v in latest.values() %}<td>{{ v }}</td>{% endfor %}</tr>
    </table>
    {% else %}
    <p>No measured numeric dataset was included in the supplied report.</p>
    {% endif %}
  </div>
  <div class="card">
    <h2>System flow</h2>
    <p>IR sensor → Arduino → relay → transmitting coil → receiver coil → battery → vehicle.</p>
  </div>
</body>
</html>
"""

@app.route("/")
def dashboard():
    rows = []
    if DATA_FILE.exists():
        with DATA_FILE.open(newline="", encoding="utf-8") as f:
            rows = list(csv.DictReader(f))
    latest = rows[-1] if rows else None
    return render_template_string(
        HTML,
        filename=DATA_FILE.name,
        count=len(rows),
        latest=latest
    )

if __name__ == "__main__":
    app.run(debug=True)
