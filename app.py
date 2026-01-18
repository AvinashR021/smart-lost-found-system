from flask import Flask, render_template, request
from collections import deque

app = Flask(__name__)

# ================= DATA STRUCTURES =================

class ItemNode:
    def __init__(self, item_id, name, category, location, status):
        self.item_id = item_id
        self.name = name
        self.category = category
        self.location = location
        self.status = status
        self.next = None

# Linked List (Main Storage)
head = None

# Hash Table (Fast Search)
item_index = {}

# Arrays
categories = ["Wallet", "Phone", "Bag", "ID Card", "Keys"]
locations = ["Library", "Lab", "Canteen", "Hostel", "Office"]

# Graph (Adjacency List)
location_graph = {
    "Library": ["Lab", "Canteen"],
    "Lab": ["Library", "Office"],
    "Canteen": ["Library", "Hostel"],
    "Hostel": ["Canteen"],
    "Office": ["Lab"]
}

# ================= FUNCTIONS =================

def add_item(item_id, name, category, location, status):
    global head
    node = ItemNode(item_id, name, category, location, status)
    node.next = head
    head = node
    item_index[item_id] = node

    with open("data.txt", "a") as f:
        f.write(f"{item_id},{name},{category},{location},{status}\n")

def bfs_locations(start):
    visited = set()
    queue = deque([start])
    result = []

    while queue:
        loc = queue.popleft()
        if loc not in visited:
            visited.add(loc)
            result.append(loc)
            for neighbor in location_graph.get(loc, []):
                queue.append(neighbor)
    return result

def find_matches(item):
    matches = []
    nearby_locations = bfs_locations(item.location)

    current = head
    while current:
        if (current.status != item.status and
            current.category == item.category and
            current.location in nearby_locations):
            matches.append(current)
        current = current.next
    return matches

# ================= ROUTES =================

@app.route("/")
def index():
    return render_template("index1.html")

@app.route("/report", methods=["GET", "POST"])
def report():
    if request.method == "POST":
        add_item(
            request.form["item_id"],
            request.form["name"],
            request.form["category"],
            request.form["location"],
            request.form["status"]
        )
    return render_template("report.html", categories=categories, locations=locations)

@app.route("/search", methods=["GET", "POST"])
def search():
    item = None
    matches = []
    if request.method == "POST":
        item = item_index.get(request.form["item_id"])
        if item:
            matches = find_matches(item)
    return render_template("search.html", item=item, matches=matches)

# ================= RUN =================

if __name__ == "__main__":
    app.run(debug=True)
