import subprocess
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.ticker as ticker
import os

# --- Конфигурация ---
solver_exe = os.path.join("x64", "Debug", "parrallel.exe")
N = 2  # число областей

# Параметры для генерации областей: [X0, Y0, x0, y0, x1, x2, hx, hy]
domains = [
    [0, 2, 0, 0, 4, 2, 0.1, 0.1],
    [4, 1, 0, 0, 4, 4, 0.2, 0.2],
]

input_files = [f"domain_{i}.txt" for i in range(N)]
output_files = [f"solution_{i}.txt" for i in range(N)]

# --- 1. Генерация входных файлов ---
for i in range(N):
    with open(input_files[i], "w") as f:
        f.write(" ".join(map(str, domains[i])) + "\n")

# --- 2. Запуск .exe с флагами -n -i ... -o ... ---
cmd = [solver_exe, "-n", str(N), "-i"] + input_files + ["-o"] + output_files + ["-s"]
#cmd = [solver_exe, "-h"] 
print("Running command:", " ".join(cmd))

try:
    subprocess.run(cmd, check=True)
except subprocess.CalledProcessError as e:
    print("Ошибка при запуске solver.exe:", e)
    exit(1)

# --- 3. Визуализация ---
fig, ax = plt.subplots(figsize=(8, 6))
ax.xaxis.set_major_locator(ticker.MultipleLocator(1))
ax.yaxis.set_major_locator(ticker.MultipleLocator(1))
ax.set_xlim(0, 8)
ax.set_ylim(0, 8)
ax.set_aspect('equal', adjustable='box')
ax.set_title(f'Решение в {N} областях')
plt.xlabel('X')
plt.ylabel('Y')
plt.grid(True, linestyle=':')

vmin = vmax = None
for filename in output_files:
    data = np.loadtxt(filename)
    u = data[:, 2]
    vmin = u.min() if vmin is None else min(vmin, u.min())
    vmax = u.max() if vmax is None else max(vmax, u.max())

for i, filename in enumerate(output_files):
    data = np.loadtxt(filename)
    x = data[:, 0]
    y = data[:, 1]
    u = data[:, 2]
    x_unique = np.unique(x)
    y_unique = np.unique(y)
    u_grid = u.reshape(len(x_unique), len(y_unique))
    
    domain_X0 = x_unique.min()
    domain_X1 = x_unique.max()
    domain_Y0 = y_unique.min()
    domain_Y1 = y_unique.max()

    pcm = ax.imshow(u_grid.T, extent=[domain_X0, domain_X1, domain_Y0, domain_Y1],
                    origin='lower', aspect='auto', alpha=0.85, vmin=vmin, vmax=vmax, cmap='hot')

plt.colorbar(pcm, ax=ax, label='u')
plt.show()
