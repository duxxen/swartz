import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.ticker as ticker

# --- Настройки глобального пространства ---
global_X0, global_X1 = 0, 8
global_Y0, global_Y1 = 0, 8

# --- Пути к файлам решений областей ---
# Можно вручную: solution_0.txt, solution_1.txt, ...
N = 2  # число областей
file_list = [f"solution_{i}.txt" for i in range(N)]  # или свой список имен

fig, ax = plt.subplots(figsize=(8,6))

ax.xaxis.set_major_locator(ticker.MultipleLocator(1))
ax.yaxis.set_major_locator(ticker.MultipleLocator(1))
ax.set_xlim(global_X0, global_X1)
ax.set_ylim(global_Y0, global_Y1)
ax.set_aspect('equal', adjustable='box')
ax.set_title('Глобальное пространство с N областями')
plt.xlabel('X (глобальные)')
plt.ylabel('Y (глобальные)')
plt.grid(True, linestyle=':')

# Для автоматического подбора цветовой шкалы
vmin = None
vmax = None

# Сначала узнаем общие min/max для всей цветовой шкалы (по желанию)
for filename in file_list:
    data = np.loadtxt(filename)
    u = data[:,2]
    vmin = u.min() if vmin is None else min(vmin, u.min())
    vmax = u.max() if vmax is None else max(vmax, u.max())

# Основной цикл по областям/файлам
for i, filename in enumerate(file_list):
    data = np.loadtxt(filename)
    x = data[:,0]
    y = data[:,1]
    u = data[:,2]
    x_unique = np.unique(x)
    y_unique = np.unique(y)
    N_x = x_unique.size
    N_y = y_unique.size
    u_grid = u.reshape(N_x, N_y)
    
    # Границы области
    domain_X0 = x_unique.min()
    domain_X1 = x_unique.max()
    domain_Y0 = y_unique.min()
    domain_Y1 = y_unique.max()
    domain_width = domain_X1 - domain_X0
    domain_height = domain_Y1 - domain_Y0

    # Отрисовать область рамкой
    rect = patches.Rectangle((domain_X0, domain_Y0), domain_width, domain_height,
                            linewidth=2, edgecolor='r', facecolor='none', linestyle='--')
    #ax.add_patch(rect)
    
    # Отрисовать тепловую карту
    pcm = ax.imshow(u_grid.T, extent=[domain_X0, domain_X1, domain_Y0, domain_Y1],
                    origin='lower', aspect='auto', alpha=0.85, vmin=vmin, vmax=vmax, cmap='hot')
    
plt.colorbar(pcm, ax=ax, label='u')
plt.show()
