using CairoMakie
using ElectronDisplay

ElectronDisplay.CONFIG.showable = showable

f = Figure()
Axis(f[1, 1])

xs = LinRange(0, 10, 100)
ys = LinRange(0, 15, 100)
zs = [cos(x) * sin(y) for x in xs, y in ys]

contour!(zs,levels=-1:0.1:1)

display(f)

println("Waiting for key press ...")
readline()