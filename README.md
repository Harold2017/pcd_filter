#

## Point cloud file filter

pcd filter based on boost spirit x3 parser (from [boost_matheval](https://github.com/hmenke/boost_matheval))

```sh
// -i: input pcd filepath (.xyz/.xyzn)
// -o: output pcd filepath (.xyz/.xyzn)
// -f: filter condition ("x**2 + y**2 < 10 && z > 0.5")
```

## Mesh file filter

```sh
// -i: input stl filepath (.ply/.stl/.obj)
// -o: output stl filepath (.ply/.stl/.obj)
// -f: filter condition ("z > 0.1 && x < 0.5")
```
