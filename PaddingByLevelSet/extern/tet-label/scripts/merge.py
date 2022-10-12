from vtkmodules.all import *
import os

input_meshes_dir = sys.argv[1]
output_mesh = sys.argv[2]

reader = vtkUnstructuredGridReader()
appendFilter = vtkAppendFilter()

filenames = [f for f in os.listdir(
    input_meshes_dir) if os.path.isfile(f) and f.endswith(".vtk")]
for file in filenames:
    reader.SetFileName(file)
    reader.Update()
    unstructured = vtkUnstructuredGrid()
    unstructured.ShallowCopy(reader.GetOutput())
    appendFilter.AddInputData(unstructured)
appendFilter.Update()

writer = vtkUnstructuredGridWriter()
writer.SetFileName(output_mesh)
writer.SetFileVersion(42)
writer.SetInputData(appendFilter.GetOutput())
writer.Write()
