import mysql.connector
import matplotlib.pyplot as plt
import time

def fetch_and_plot_data():
    
    # Establish connection to the database
    conexion1 = mysql.connector.connect(host="localhost",
                                        user="root",
                                        passwd="",
                                        database="sensors")
    cursor1 = conexion1.cursor()

    # Execute a SELECT query to fetch data from a specific table
    cursor1.execute("SELECT * FROM test")

    # Fetch all rows from the result set
    rows = cursor1.fetchall()

    # Extract datetime values from the fourth position of each row
    datetime_values = [row[3] for row in rows]

    # Extract data for Data1 and Data2
    data1 = [row[1] for row in rows]
    data2 = [row[2] for row in rows]

    # Create subplots
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
    

    # Plot Data1 vs Datetime in subplot 1
    ax1.plot(datetime_values, data1, marker='o', linestyle='-')
    ax1.set_xlabel('Datetime')
    ax1.set_ylabel('Temperatura (°C)')
    ax1.tick_params(axis='x', rotation=45)

    # Plot Data2 vs Datetime in subplot 2
    ax2.plot(datetime_values, data2, marker='o', linestyle='-')
    ax2.set_xlabel('Datetime')
    ax2.set_ylabel('Humedad (%)')
    ax2.tick_params(axis='x', rotation=45)

    # Show the plot
    plt.tight_layout()
    plt.show()
    
    # Close the cursor and connection
    cursor1.close()
    conexion1.close()

# Main loop to execute fetch_and_plot_data() every 30 seconds
while True:
    fetch_and_plot_data()
    # Sleep for 30 seconds
    time.sleep(10)
    plt.close()
