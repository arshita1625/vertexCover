import re
import sys
from intersect import *

def insert_vertex(vertices, pt):
    modified_point = Vertex(normalize_coordinate(pt.x), normalize_coordinate(pt.y))
    vertices.add(modified_point)

def insert_edge(edges, vertices, pt1, pt2):
    modified_point1 = Vertex(normalize_coordinate(pt1.x), normalize_coordinate(pt1.y))
    modified_point2 = Vertex(normalize_coordinate(pt2.x), normalize_coordinate(pt2.y))
    
    if modified_point1 != modified_point2:
        edges.append((modified_point1, modified_point2))
        vertices.add(modified_point1)
        vertices.add(modified_point2)

def graph_generator(segments, intersect_points):

    nodes = set()  
    paths = [] 

    for point in intersect_points:
        insert_vertex(nodes, point)

    path_coor = set()

    for seg in segments:
        for segment in seg.list_segments:
            path_coor.add(segment.src)
            path_coor.add(segment.dest)

    neighbor_src = {}
    neighbor_dest = {}

    for coor in intersect_points:
        for seg in segments:
            seg_lines = seg.list_segments
            for segment in seg_lines:

                if point_lies_on_segment(segment.src, coor, segment.dest):
                    if len(intersect_points) == 1:
                        if coor != segment.src:
                            insert_edge(paths, nodes, coor, segment.src)
                        if coor != segment.dest:
                            insert_edge(paths, nodes, coor, segment.dest)
                        insert_vertex(nodes, segment.src)
                        insert_vertex(nodes, segment.dest)
                        continue

                    count = sum(1 for pnt in intersect_points if pnt != coor and point_lies_on_segment(segment.src, pnt, segment.dest))
                    if count == 0:
                        if coor != segment.src:
                            insert_edge(paths, nodes, coor, segment.src)
                        if coor != segment.dest:
                            insert_edge(paths, nodes, coor, segment.dest)
                        insert_vertex(nodes, segment.src)
                        insert_vertex(nodes, segment.dest)

                    for other_poin in intersect_points:
                        if other_poin == coor:
                            continue

                        if point_lies_on_segment(segment.src, other_poin, segment.dest):
                            if point_lies_on_segment(coor, other_poin, segment.src):
                                segment_edge = Line(coor, other_poin)
                                neighbor_src[compute_dist(coor, other_poin)] = segment_edge
                            else:
                                found_other_point = any(
                                    point_lies_on_segment(coor, pnt, segment.src)
                                    for pnt in intersect_points
                                    if pnt != coor
                                )

                                if not found_other_point and coor != segment.src:
                                    insert_edge(paths, nodes, coor, segment.src)
                                    insert_vertex(nodes, segment.src)

                            if point_lies_on_segment(coor, other_poin, segment.dest):
                                segment_edge = Line(coor, other_poin)
                                neighbor_dest[compute_dist(coor, other_poin)] = segment_edge
                            else:
                                found_other_point = any(
                                    point_lies_on_segment(coor, pnt, segment.dest)
                                    for pnt in intersect_points
                                    if pnt != coor
                                )

                                if not found_other_point and coor != segment.dest:
                                    insert_edge(paths, nodes, coor, segment.dest)
                                    insert_vertex(nodes, segment.dest)

                if neighbor_src:
                    source_keys = min(neighbor_src.keys())
                    source_vals = neighbor_src[source_keys]
                    if source_vals.src != source_vals.dest:
                        insert_edge(paths, nodes, source_vals.src, source_vals.dest)

                if neighbor_dest:
                    dest_keys = min(neighbor_dest.keys())
                    dest_vals = neighbor_dest[dest_keys]
                    if dest_vals.src != dest_vals.dest:
                        insert_edge(paths, nodes, dest_vals.src, dest_vals.dest)

                neighbor_src.clear()
                neighbor_dest.clear()

    paths = filter_edges(paths)

    return nodes, paths

def generate_vertices(vertex_set):
    vertices_list = {v_index + 1: ver for v_index, ver in enumerate(vertex_set)}
    print("V = {")
    for id, vertex in vertices_list.items():
        coor_x = normalize_coordinate(round(vertex.x, 2))
        coor_y = normalize_coordinate(round(vertex.y, 2))
        print(f" {id}:  ({coor_x}, {coor_y})")
    print("}")
    
    return vertices_list

            
def generate_edges(edges, vertex_set):
    print('E = {')
    def get_vertex_index(vertex):
        for idx, current_vertex in vertex_set.items():
            if current_vertex == vertex:
                return idx
    for idx, ed in enumerate(edges):
        start = get_vertex_index(ed[0])
        end = get_vertex_index(ed[1])
        join = '' if idx == len(edges) - 1 else ','
        print(f"  <{start},{end}>{join}")
    print("}")

def filter_edges(edges_list):
    unique_edges = set()
    for i in edges_list:
        modified = (i[1], i[0])
        if i not in unique_edges and modified not in unique_edges:
            unique_edges.add(i)
    return list(unique_edges)



def create_street(street_info, points, streets):
    parts = street_info.split('"')
    street_name = parts[1].strip().lower()

    if not street_name or street_name.isspace():
        print("Error: No street name", file=sys.stderr)
        return

    existing_street_names = any(street.name == street_name for street in streets)

    if existing_street_names:
        print("Error: Street already exists!", file=sys.stderr)
        return

    if len(points) < 2:
        print("Error: At least 2 coordinates are required", file=sys.stderr)
        return

    routes = []
    
    for coor in zip(points, points[1:]):
        start_point = coor[0].strip("()").split(",")
        end_point = coor[1].strip("()").split(",")
        start = Vertex(float(start_point[0]), float(start_point[1]))
        end = Vertex(float(end_point[0]), float(end_point[1]))

        if start == end:
            print("Error: Duplicate coordinates found", file=sys.stderr)
            return

        routes.append(Line(start, end))

    streets.append(Street(street_name, routes))


def modify_street(street_info, points, streets):
    parts = street_info.split('"')
    street_name = parts[1].strip().lower()

    if not street_name or street_name.isspace():
        print("Error: No street name", file=sys.stderr)
        return

    if len(points) < 2:
        print("Error: At least 2 coordinates are required", file=sys.stderr)
        return

    existing_street_names = [street.name for street in streets]

    if street_name not in existing_street_names:
        print("Error: Street does not exist", file=sys.stderr)
        return

    streets[:] = [street for street in streets if street.name != street_name]

    updated_routes = []
    
    for coor in zip(points, points[1:]):
        start_point = coor[0].strip("()").split(",")
        end_point = coor[1].strip("()").split(",")
        start = Vertex(float(start_point[0]), float(start_point[1]))
        end = Vertex(float(end_point[0]), float(end_point[1]))

        if start == end:
            print("Error: Duplicate coordinates found", file=sys.stderr)
            return

        updated_routes.append(Line(start, end))

    streets.append(Street(street_name, updated_routes))


def remove_street(street_info, streets):
    if not street_info.strip():
        print("Error: No street name", file=sys.stderr)
        return

    parts = street_info.split('"')
    street_name = parts[1].strip().lower()

    if not street_name or street_name.isspace():
        print("Error: No street name", file=sys.stderr)
        return

    existing_street_names = [street.name for street in streets]

    if street_name not in existing_street_names:
        print("Error: Street does not exist", file=sys.stderr)
        return

    streets[:] = [street for street in streets if street.name != street_name]


def generate_output(streets):
    intersections = find_intersections(streets)
    vertices, edges = graph_generator(streets, intersections)
    vertex_list = generate_vertices(vertices)
    generate_edges(edges, vertex_list)

def get_elements(match):
    command = match.group(1).strip()
    label = match.group(2).strip() if match.group(2) else ""
    data = match.group(3).strip() if match.group(3) else ""
    return command, label, data

def validate(input, data):
    if '"(' in input:
        return "Incorrect format"
    if ')(' in data:
        return "Incorrect format"
    return None


def run():
    streets = []
    
    while True:
        input = sys.stdin.readline()

        if not input.strip():
            break
        
        operation = r'\s*(add|mod|rm|gg)\s+((?:\s*"[a-zA-Z ]+?"\s*?)?)((?:\(\s*-?\s*[0-9]+?\s*,\s*-?\s*[0-9]+?\s*\)\s*?)*)?\s*$'
        match = re.match(operation, input)

        if match:
            command, label, data = get_elements(match)
            errorr = validate(input, data)
            if errorr:
                print(f"Error: {errorr}", file=sys.stderr)
                continue
            
            if command in ['add', 'mod']:
                if not label:
                    print("Error: Street name missing", file=sys.stderr)
                    continue
                
                if len(data) == 0:
                    print("Error: Missing vertices", file=sys.stderr)
                    continue
            
            elif command == 'rm' and not label:
                    print("Error: Cannot remove a non existent street", file=sys.stderr)
                    continue
            
            elif command == 'gg' and (label or data):
                   print("Error: Incorrect input", file=sys.stderr)
                   continue
        
            if data:
                coordinates = re.findall(r'\(\s*-?\s*[0-9]+?\s*,\s*-?\s*[0-9]+?\s*\)', data)
                if not coordinates:
                    print("Error: Invalid format", file=sys.stderr)
                    continue
        else: 
           print("Error: Incorrect format", file=sys.stderr)
           continue

        try:
            if command == 'add':
                create_street(label, coordinates, streets)
            elif command == 'mod':
                modify_street(label, coordinates, streets)
            elif command == 'rm':
                remove_street(label, streets)
            elif command == 'gg':
                generate_output(streets)

        except Exception as e:
            print(f"Error: {str(e)}", file=sys.stderr)
            continue
    
    sys.exit(0)

if __name__ == "__main__":
    run()