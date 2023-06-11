using CairoMakie


xs = range(0, 10, length = 30)
ys = 0.5 .* sin.(xs)

scatter(xs, ys)