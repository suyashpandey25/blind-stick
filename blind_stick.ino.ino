#include <Arduino.h>
#include <ESP_Mail_Client.h> // Include the ESP Mail Client library
#include <ESP8266WiFi.h> // Include the Wi-Fi library for ESP8266

// Wi-Fi credentials
const char* ssid = "PSITPSIT"; // Your Wi-Fi SSID
const char* password = "1234567890"; // Your Wi-Fi password

// Pin configuration
const int trigPin = D1; // Trigger pin for ultrasonic sensor
const int echoPin = D2; // Echo pin for ultrasonic sensor
const int buzzerPin = D3; // Buzzer pin

// SMTP credentials
SMTPSession smtp;
SMTP_Message message;

const char* smtp_server = "smtp.gmail.com"; // SMTP server address
const int smtp_port = 465; // SMTP server port for SSL
const char* smtp_user = "30827csiot@gmail.com"; // Your email address
const char* smtp_password = "khvscukmuwgvdaji"; // Your email password

Session_Config session_config;

void setupSMTPConfig() {
  session_config.server.host_name = smtp_server;
  session_config.server.port = smtp_port;
  session_config.login.email = smtp_user;
  session_config.login.password = smtp_password;
  session_config.login.user_domain = "gmail.com";
}

// Function to calculate distance from ultrasonic sensor
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2; // Distance in cm
  return distance;
}

// Function to send email notification
void sendEmailNotification(float distance) {
  message.sender.name = "NodeMCU"; // Sender name
  message.sender.email = smtp_user;
  message.subject = "Obstacle Detected";
  message.addRecipient("Recipient", "pandeysuyash181@gmail.com"); // Name and email of the recipient
  message.text.content = "An obstacle was detected at a distance of " + String(distance) + " cm.";

  smtp.connect(&session_config); // Connect to the SMTP server

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Failed to send email: " + smtp.errorReason());
  }

  smtp.closeSession(); // Close the SMTP session
}

void setup() {
  Serial.begin(115200); // Start serial communication
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");

  setupSMTPConfig(); // Set up SMTP configuration

  smtp.debug(1); // Enable debugging messages
}

void loop() {
  float distance = getDistance();

  if (distance <= 10) { // If an object is within 10 cm
    digitalWrite(buzzerPin, HIGH);
    delay(2000); // Turn on the buzzer
    digitalWrite(buzzerPin, LOW);
    // Send email notification
    sendEmailNotification(distance);

  } else {
    digitalWrite(buzzerPin, LOW); // Turn off the buzzer
  }

  delay(200); // Short delay before the next measurement
}
