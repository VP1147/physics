# TermiGraph - generate graphs using only text characters
# on a terminal screen.
# (C) 2023 Vinicius Pavao


import math as m

def f(x):
	return m.sin(x**2)

# h - Altura (unidades)
# l - Comprimento (unidades)
# f - Funcao para plotar - f(x)
# k - Caracteres por unidade
def graph(h, l, f, k):
	i = h
	while i > -h: 
		j = 0
		while j <= l:
			try:
				if i < f(j+1/(2*k)): 
					print("#", end="")
				else: 
					print(" ", end="")
			except ValueError: print(" ", end="")
			j += 1/k
		print(" {:.1f}".format(i))
		i -= 1/k
	j = 0
	while j <= l:
		if j % k == 0: print("|", end="")
		else: print(" ", end="")
		j += 1/k
	print()

nc = 60			# Numero de caracteres
zf = 10 			# Fator de zoom (par)
graph(1, nc/zf, f, zf)
