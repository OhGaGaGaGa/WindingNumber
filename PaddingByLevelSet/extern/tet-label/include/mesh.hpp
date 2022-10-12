#ifndef MESH_TOOLS_MESH_H
#define MESH_TOOLS_MESH_H

#include <map>
#include <vector>
#include <cassert>
#include <cstddef>

namespace mesh_tools {
namespace mesh {

enum CellType { UNKNOW = 0, TRIANGLE = 5, TET = 10, HEX = 12, WEDGE = 13, PYRAMID = 14 };

template <typename Vertex> struct Cell {
CellType type;
std::vector<Vertex> vertices;
Cell(CellType type, const std::vector<Vertex> &vertices);
};

template <typename Point> class Mesh {
public:
std::size_t AddVertex(const Point &point);
std::size_t AddCell(const Cell<std::size_t> &cell);
std::size_t NumVertices() const;
std::size_t NumCells() const;
const Point &GetPoint(std::size_t id) const;
const Cell<std::size_t> &GetCell(std::size_t id) const;

int RemoveIsolatedVertices();
private:
std::vector<Point> _vertices;
std::vector<Cell<std::size_t>> _cells;
};

// ===== Implement =====
template <typename Vertex>
Cell<Vertex>::Cell(CellType type, const std::vector<Vertex> &vertices)
: type(type), vertices(vertices){};

template <typename Point>
std::size_t Mesh<Point>::AddVertex(const Point &point) {
std::size_t id = _vertices.size();
_vertices.push_back(point);
return id;
}
template <typename Point>
std::size_t Mesh<Point>::AddCell(const Cell<std::size_t> &cell) {
std::size_t id = _cells.size();
_cells.push_back(cell);
return id;
};
template <typename Point> std::size_t Mesh<Point>::NumVertices() const {
return _vertices.size();
}
template <typename Point> std::size_t Mesh<Point>::NumCells() const {
return _cells.size();
}
template <typename Point> const Point &Mesh<Point>::GetPoint(size_t id) const {
assert(id < _vertices.size());
return _vertices[id];
}
template <typename Point>
const Cell<size_t> &Mesh<Point>::GetCell(size_t id) const {
assert(id < _cells.size());
return _cells[id];
}

template <typename Point> int Mesh<Point>::RemoveIsolatedVertices() {
std::vector<bool> is_isolated(NumVertices(), true);
for (size_t i = 0; i < NumCells(); ++i) {
for (size_t v : GetCell(i).vertices) {
is_isolated[v] = false;
}
}
std::map<int, int> new_indexes;
std::vector<Point> vertices;
for (int p = 0, q = 0; p < NumVertices(); ++p) {
if (!is_isolated[p]) {
new_indexes[p] = q;
++q;
vertices.push_back(_vertices[p]);
}
}
for (auto &cell : _cells) {
for (auto &v : cell.vertices) {
auto it = new_indexes.find(v);
assert(it != new_indexes.end());
v = it -> second;
}
}
_vertices = vertices;
return 0;
}

} // namespace mesh
} // namespace mesh_tools
#endif