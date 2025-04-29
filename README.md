

### 📦 Arduino (MWS to Firebase)

# MWS to Firebase (IoT Weather Monitoring with ESP32)

Proyek ini merupakan bagian dari sistem monitoring cuaca berbasis IoT menggunakan ESP32, Firebase Realtime Database, dan sensor lingkungan seperti DHT11 (suhu & kelembapan), LDR (intensitas cahaya), serta sensor hujan. ESP32 akan mengirimkan data secara real-time ke Firebase untuk divisualisasikan pada dashboard web.

## 🔧 Komponen Hardware

- ESP32 Dev Module  
- Sensor DHT11  
- Sensor LDR  
- Sensor Hujan  
- RTC Module (optional, bisa pakai NTP)
- Breadboard & kabel jumper  
- Koneksi Wi-Fi  

## 🧠 Fitur Utama

- Membaca data suhu dan kelembapan menggunakan DHT11  
- Mendeteksi hujan menggunakan sensor hujan  
- Menentukan kondisi cuaca (mendung / cerah / malam) berdasarkan intensitas cahaya (LDR) dan waktu  
- Mengirim data ke Firebase Realtime Database  

## 📁 Struktur Firebase (Realtime Database)

```json
MWS: {
  Temp: 29.3,
  Hum: 70,
  Rain: true,
  Weather: "Mendung",
  Time: "08:45"
}
```

## 📦 Library yang Digunakan

- `WiFi.h`
- `Firebase_ESP_Client.h`
- `Adafruit_Sensor.h`
- `DHT.h`
- `RTClib.h` (jika pakai RTC fisik)
- `NTPClient.h` (jika pakai waktu internet)

## 🔑 Setup Secrets

Buat file `secrets.h` dan isi dengan:

```cpp
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"
#define API_KEY "YourFirebaseWebAPIKey"
#define USER_EMAIL "user@email.com"
#define USER_PASSWORD "yourpassword"
#define DATABASE_URL "https://yourproject.firebaseio.com/"
```

## 🚀 Cara Menjalankan

1. Upload sketch ke ESP32 via Arduino IDE.  
2. Pastikan koneksi Wi-Fi tersedia.  
3. Cek data yang masuk ke Firebase.  
4. Dashboard web akan otomatis menampilkan data.

## 🧪 Validasi Logika Cuaca

Logika cuaca akan mempertimbangkan:
- Jika **LDR gelap** dan jam menunjukkan siang (06:00–17:59) → maka **mendung**  
- Jika **LDR gelap** dan suhu ≤ 28°C → maka **malam**  
- Jika **LDR terang** → maka **cerah**

---
