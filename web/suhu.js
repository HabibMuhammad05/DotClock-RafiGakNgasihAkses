const refresh_time = 1000;

async function load_sensor() {
    try {
    const response = await fetch(endpoint_url + "/getsensor");
    if (!response.ok) {
        throw new Error("Network error!");
    }
    const data = await response.json();
    console.log(data);

    // List ID elemen yang mau di-update
    const mapping = {
        sensor1: data.sensor1 + " °C",
        sensor2: data.sensor2 + " %",
    };

    // Loop untuk periksa ID div sebelum update
    Object.entries(mapping).forEach(([id, value]) => {
        const element = document.getElementById(id);
        if (element) {
        element.textContent = value ?? "-";
        }
    });
    } catch (error) {
    console.error("Error loading sensors:", error);
    }
}

// Tunggu sampai semua elemen dimuat sebelum mengambil status
document.addEventListener("DOMContentLoaded", function () {
    load_sensor();
  setInterval(load_sensor, refresh_time); // Perbaikan di sini
});
