import mysql.connector
import matplotlib.pyplot as plt
import time

def fetch_and_plot_data():
    
    # Establecer conexión con la base de datos
    conexion1 = mysql.connector.connect(host="localhost",
                                        user="root",
                                        passwd="",
                                        database="sensors")
    cursor1 = conexion1.cursor()

    # Ejecutar una consulta SELECT para obtener datos de una tabla específica
    cursor1.execute("SELECT * FROM test")

    # Obtener todas las filas del conjunto de resultados
    rows = cursor1.fetchall()

    # Extraer valores de fecha y hora desde la cuarta posición de cada fila
    datetime_values = [row[5] for row in rows]

    # Extraer datos para Temperatura, Humedad, CO y Humo
    temperatura = [row[1] for row in rows]
    humedad = [row[2] for row in rows]
    co = [row[3] for row in rows]
    humo = [row[4] for row in rows]

    # Crear subplots
    fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(14, 10))

    # Graficar Temperatura vs Datetime en el subplot 1
    ax1.plot(datetime_values, temperatura, marker='o', linestyle='-')
    ax1.set_xlabel('Datetime')
    ax1.set_ylabel('Temperatura (°C)')
    ax1.set_title('Temperatura vs Datetime')
    ax1.tick_params(axis='x', rotation=45)

    # Graficar Humedad vs Datetime en el subplot 2
    ax2.plot(datetime_values, humedad, marker='o', linestyle='-')
    ax2.set_xlabel('Datetime')
    ax2.set_ylabel('Humedad (%)')
    ax2.set_title('Humedad vs Datetime')
    ax2.tick_params(axis='x', rotation=45)

    # Graficar CO vs Datetime en el subplot 3
    ax3.plot(datetime_values, co, marker='o', linestyle='-')
    ax3.set_xlabel('Datetime')
    ax3.set_ylabel('CO (ppm)')
    ax3.set_title('CO vs Datetime')
    ax3.tick_params(axis='x', rotation=45)

    # Graficar Humo vs Datetime en el subplot 4
    ax4.plot(datetime_values, humo, marker='o', linestyle='-')
    ax4.set_xlabel('Datetime')
    ax4.set_ylabel('Humo (ppm)')
    ax4.set_title('Humo vs Datetime')
    ax4.tick_params(axis='x', rotation=45)

    # Mostrar la gráfica
    plt.tight_layout()
    plt.show()
    
    # Cerrar el cursor y la conexión
    cursor1.close()
    conexion1.close()

# Llamar a la función para ejecutar la obtención y graficación de datos
fetch_and_plot_data()

