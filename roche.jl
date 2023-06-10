function roche_distance(R, M, m)
	d = R * (2*M/m)^(1/3)
	return d
end

earth_radius = 6378					# Earth radius (Km)
earth_mass = 5.97e24				# Earth mass (kg)

moon_mass = 7.34e22					# Moon mass (Kg)

print("The Moon's Roche limit is ")
println(round(roche_distance(earth_radius, earth_mass, moon_mass), digits=3), " Km")
