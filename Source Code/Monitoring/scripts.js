const temperatureData = {
    labels: ['Day 1', 'Day 2', 'Day 3', 'Day 4', 'Day 5'],
    datasets: [{
        label: 'Temperature',
        data: [15, 18, 21, 19, 17],
        backgroundColor: 'rgba(75, 192, 192, 0.2)',
        borderColor: 'rgba(75, 192, 192, 1)',
        borderWidth: 1
    }]
};

const humidityData = {
    labels: ['Day 1', 'Day 2', 'Day 3', 'Day 4', 'Day 5'],
    datasets: [{
        label: 'Humidity',
        data: [50, 55, 60, 65, 70],
        backgroundColor: 'rgba(153, 102, 255, 0.2)',
        borderColor: 'rgba(153, 102, 255, 1)',
        borderWidth: 1
    }]
};

const temperatureConfig = {
    type: 'bar',
    data: temperatureData,
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
};

const humidityConfig = {
    type: 'bar',
    data: humidityData,
    options: {
        scales: {
            y:{
                beginAtZero: true
            }
        }
    }
};

const temperatureChart = new Chart(
    document.getElementById('temperature-chart'),
    temperatureConfig
);

const humidityChart = new Chart(
    document.getElementById('humidity-chart'),
    humidityConfig
);