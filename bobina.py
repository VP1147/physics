# Calcula a indutancia de uma bobina com nucleo
# de ar a partir do seu diametro (mm) e comprimento (mm),
# numero de espiras e diametro do fio. Resultado em μH.

import math as m

def indut(d, l, N):				# d - Diametro da bobina (mm)
								# l - Comprimento do nucleo (mm)
								# N - Numero de voltas na bobina
	μ = 1.256e-6		# Ar
	A = m.pi*(d/2)**2
	L = (N**2 * A * μ)/l
	print("A = {:.2f} mm^2".format(1e6*A))
	print("L = {:.3f} μH".format(1e6*L))

indut(0.2e-2, 2e-2, 400)

