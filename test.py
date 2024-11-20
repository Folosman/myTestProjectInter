import numpy as np

# Координаты центра окружности и радиуса
center = np.array([0.0133182, 0.69882, 2.04585])
radius = 3.05794

# Координаты точек
points = {
    "A": np.array([0.5, 3, 4]),
    "B": np.array([1, -2, 1]),
    "C": np.array([-2, 3, 2])
}

# Проверяем расстояние от каждой точки до центра
results = {point: np.linalg.norm(coords - center) for point, coords in points.items()}

# Определяем, лежат ли точки на окружности
on_circle = {point: np.isclose(distance, radius, atol=1e-5) for point, distance in results.items()}

results, on_circle
