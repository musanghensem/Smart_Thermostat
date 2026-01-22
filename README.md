# Smart Thermostat with Intelligent Control (oneM2M)

**Capstone Project | Semester Oct 2025**
**Theme:** Securing Smart Home Application using IoT Standards

## 📌 Project Overview
This project focuses on the design and implementation of a **Secure Smart Thermostat System** that adheres to the **oneM2M international standard**. The system autonomously regulates room temperature using a "Smart Decision Engine" while ensuring end-to-end security through a **Secure Gateway Architecture**.

The primary goal is to demonstrate "Security by Design" in IoT, mitigating common threats such as Man-in-the-Middle (MITM) attacks and unauthorized access using **TLS/SSL encryption** and strict **Access Control Policies (ACPs)**.

## 🚀 Key Features

### Functional Features
* **Real-Time Monitoring:** Continuously tracks temperature and humidity via DHT11 sensors.
* **Intelligent Control Logic:**
    * **Basic Mode:** Automatic activation of Heater/Fan based on temperature thresholds.
    * **Eco Mode:** Energy-saving profile that reduces fan speed and optimizes ventilation.
* **Interactive Dashboard:** A Node-RED based GUI for real-time visualization and remote control.
* **Standardized Interoperability:** Uses **Mobius (oneM2M)** for standardized data storage and device management.

### Security Features
* **Secure Gateway (Reverse Proxy):** Implements **NGINX** to enforce **TLS 1.2/1.3 (HTTPS)** encryption for all traffic.
* **Identity Management:** Enforces **Originator IDs (AE-ID)** validation for all API requests.
* **Access Control:** Strict **oneM2M ACPs** prevent unauthorized retrieval or modification of data containers.
* **Authentication:** Application-level login required to access the control dashboard.

---

## 📂 Repository Structure

* `/firmware` - Contains the C++ code (`.ino`) for the ESP32 microcontroller.
* `/middleware` - Contains the Node-RED flows (`flows.json`) for the dashboard and logic engine.
* `/gateway` - Contains the NGINX configuration (`nginx.conf`) and security setup.
* `/docs` - Project diagrams, flowcharts, and circuit photos.

---

## 🛠️ Technology Stack

| Category | Technology Used |
| :--- | :--- |
| **Hardware** | ESP32 Dev Board, DHT11, Relay Module, Servo (SG90), DC Fan |
| **IoT Platform** | Mobius (oneM2M Standard) |
| **Middleware** | Node-RED |
| **Security Gateway** | NGINX Web Server (Reverse Proxy) |
| **Cryptography** | OpenSSL (X.509 Certificate Generation) |
| **Database** | MySQL |

---

## ⚙️ Installation & Setup

### 1. Hardware Setup
* Connect the DHT11 Sensor to GPIO 4.
* Connect the Relay Module (Heater) to GPIO 5.
* Connect the Servo Motor (Ventilation) to GPIO 18.
* Flash the firmware located in `/firmware/Smart_Thermostat.ino` using Arduino IDE.

### 2. Secure Gateway (NGINX)
* Install NGINX on your local server/PC.
* Copy the `nginx.conf` file from `/gateway` to your NGINX `conf` directory.
* Generate your SSL certificates:
    ```bash
    openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout server.key -out server.crt
    ```
* Start NGINX to enable the HTTPS tunnel on Port 443.

### 3. Middleware (Node-RED)
* Install Node-RED: `npm install -g --unsafe-perm node-red`
* Import the flow: Copy the content of `/middleware/flows.json` and import it into your Node-RED editor.
* Deploy the flow and navigate to `https://localhost/ui` to view the dashboard.

---

## 📸 System Architecture

![System Architecture](docs/architecture_diagram.png)


---

## 👥 Team Members

* **Hazman** - [52224123618]
* **Izwan** - [52224123217]
* **Eiqmal** - [52224123015]

---

## 📜 License
This project is for educational purposes as part of the UniKL IIB20804 IoT Application Security Capstone.
