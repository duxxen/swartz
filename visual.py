import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import matplotlib.ticker as ticker
import subprocess
import os

def generate_domain(filename, x0, y0, x1, y1, hx, hy):
    with open(filename, 'w') as f:
        f.write(f'{x0} {y0} {x1} {y1} {hx} {hy}\n')

def run_solver(exe_path):
    subprocess.run([exe_path], check=True)

def main():
    # --- Настройки областей ---
    domains = [
        ('domain_0.txt', 2, 2, 10, 6, 0.1, 0.1),
        ('domain_1.txt', 6, 0, 14, 8, 0.1, 0.1)
    ]

    # Генерация файлов областей
    for filename, *params in domains:
        generate_domain(filename, *params)

    # Запуск solver (один раз для всех областей)
    run_solver('x64\\Debug\\swartz.exe')

    N = len(domains)
    file_list = [f"solution_{i}.txt" for i in range(N)]

    # Собираем все данные в одну общую таблицу
    all_data = []
    for filename in file_list:
        data = np.loadtxt(filename)
        all_data.append(data)
    all_data = np.vstack(all_data)

    # Получаем уникальные координаты для построения общей сетки
    x_unique = np.unique(all_data[:, 0])
    y_unique = np.unique(all_data[:, 1])

    X, Y = np.meshgrid(x_unique, y_unique, indexing='ij')
    U = np.zeros_like(X)

    # Заполняем сетку: пропуски заполняем минимальным значением
    data_dict = {(x, y): val for x, y, val in all_data}
    min_val = all_data[:, 2].min()

    for i, x in enumerate(x_unique):
        for j, y in enumerate(y_unique):
            U[i, j] = data_dict.get((x, y), min_val)

    # Рисуем
    fig, ax = plt.subplots(figsize=(8, 6))
    ax.xaxis.set_major_locator(ticker.MultipleLocator(1))
    ax.yaxis.set_major_locator(ticker.MultipleLocator(1))
    ax.set_aspect('equal', adjustable='box')
    ax.set_title('Глобальная тепловая карта')
    plt.xlabel('X (глобальные)')
    plt.ylabel('Y (глобальные)')
    plt.grid(True, linestyle=':')

    pcm = ax.imshow(U.T, extent=[x_unique.min(), x_unique.max(), y_unique.min(), y_unique.max()],
                    origin='lower', aspect='auto', cmap='hot')

    # Обводка областей и подписи углов
    offset = 0.1
    for _, x0, y0, x1, y1, hx, hy in domains:
        width = x1 - x0
        height = y1 - y0
        rect = patches.Rectangle((x0, y0), width, height, 
                                  linewidth=2, edgecolor='r', facecolor='none', linestyle='--')
        ax.add_patch(rect)
        ax.text(x0 + offset, y0 + offset, f"({x0}, {y0})", fontsize=8, color='blue', ha='left', va='bottom')
        ax.text(x1 - offset, y1 - offset, f"({x1}, {y1})", fontsize=8, color='blue', ha='right', va='top')

    # Определяем глобальные границы с отступом
    global_x0 = min(x0 for _, x0, y0, x1, y1, hx, hy in domains)
    global_x1 = max(x1 for _, x0, y0, x1, y1, hx, hy in domains)
    global_y0 = min(y0 for _, x0, y0, x1, y1, hx, hy in domains)
    global_y1 = max(y1 for _, x0, y0, x1, y1, hx, hy in domains)

    margin_x = 0.05 * (global_x1 - global_x0)
    margin_y = 0.05 * (global_y1 - global_y0)

    ax.set_xlim(global_x0 - margin_x, global_x1 + margin_x)
    ax.set_ylim(global_y0 - margin_y, global_y1 + margin_y)

    plt.colorbar(pcm, ax=ax, label='u')
    plt.show()

if __name__ == '__main__':
    main()