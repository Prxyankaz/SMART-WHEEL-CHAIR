package com.example.smart_wheelchair

import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Card
import androidx.compose.material3.CardDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.smart_wheelchair.ui.theme.Smart_WheelchairTheme
import com.google.firebase.database.DataSnapshot
import com.google.firebase.database.DatabaseError
import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase
import com.google.firebase.database.ValueEventListener

class MainActivity : ComponentActivity() {

    private lateinit var database: DatabaseReference

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()

        // Initialize Firebase Database
        database = FirebaseDatabase.getInstance().reference

        setContent {
            Smart_WheelchairTheme {
                MainDashboard(database) { lat, lng ->
                    openMap(lat, lng)
                }
            }
        }
    }

    private fun openMap(lat: Double, lng: Double) {
        val intent = Intent(this, MapActivity::class.java).apply {
            putExtra("latitude", lat)
            putExtra("longitude", lng)
        }
        startActivity(intent)
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun MainDashboard(database: DatabaseReference, openMap: (Double, Double) -> Unit) {
    var acceleration by remember { mutableStateOf("0.0 m/s²") }
    var gyroscope by remember { mutableStateOf("0.0 rad/s") }
    var heartRate by remember { mutableStateOf("0 bpm") }
    var fallDetected by remember { mutableStateOf(false) }

    val latitude = 10.904048600061039
    val longitude = 76.8983838821322
    Log.d("MapActivity", "Latitude: $latitude, Longitude: $longitude")

    LaunchedEffect(Unit) {
        // Fetch all fall_data values dynamically
        database.child("fall_data").addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                for (data in snapshot.children) { // Loop through all branches dynamically
                    for (child in data.children) {
                        val accelX = child.child("accel_x").value.toString().toDoubleOrNull() ?: 0.0
                        val accelY = child.child("accel_y").value.toString().toDoubleOrNull() ?: 0.0
                        val accelZ = child.child("accel_z").value.toString().toDoubleOrNull() ?: 0.0
                        val gyroX = child.child("gyro_x").value.toString().toDoubleOrNull() ?: 0.0
                        val gyroY = child.child("gyro_y").value.toString().toDoubleOrNull() ?: 0.0
                        val gyroZ = child.child("gyro_z").value.toString().toDoubleOrNull() ?: 0.0
                        fallDetected = child.child("fall_alert").value == true

                        // Display updated sensor values
                        acceleration = "X: $accelX, Y: $accelY, Z: $accelZ m/s²"
                        gyroscope = "X: $gyroX, Y: $gyroY, Z: $gyroZ rad/s"
                    }
                }
            }

            override fun onCancelled(error: DatabaseError) {
                // Handle database error
            }
        })

        // Fetch all readings dynamically
        database.child("readings").addValueEventListener(object : ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                for (child in snapshot.children) {
                    val heartRateValue = child.child("heartRate").value.toString().toDoubleOrNull() ?: 0.0
                    heartRate = "$heartRateValue bpm"
                }
            }

            override fun onCancelled(error: DatabaseError) {
                // Handle database error
            }
        })
    }

    Scaffold(
        modifier = Modifier.fillMaxSize(),
        topBar = {
            TopAppBar(
                title = { Text("Smart Wheelchair Dashboard") },
                colors = TopAppBarDefaults.mediumTopAppBarColors(containerColor = Color(0xFF1976D2))
            )
        }
    ) { innerPadding ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(innerPadding)
                .padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(12.dp)
        ) {
            SensorCard("Acceleration", acceleration)
            SensorCard("Gyroscope", gyroscope)
            SensorCard("Heart Rate", heartRate)

            if (fallDetected) {
                FallWarningCard(latitude, longitude, openMap)
            }
        }
    }
}

@Composable
fun SensorCard(title: String, value: String) {
    Card(
        shape = RoundedCornerShape(12.dp),
        elevation = CardDefaults.cardElevation(8.dp),
        modifier = Modifier.fillMaxWidth()
    ) {
        Column(
            modifier = Modifier
                .padding(16.dp)
                .fillMaxWidth()
        ) {
            Text(
                text = title,
                fontSize = 18.sp,
                fontWeight = FontWeight.Bold,
                color = Color(0xFF1976D2)
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = value,
                fontSize = 16.sp,
                color = Color.DarkGray
            )
        }
    }
}

@Composable
fun FallWarningCard(lat: Double, lng: Double, openMap: (Double, Double) -> Unit) {
    Card(
        shape = RoundedCornerShape(12.dp),
        modifier = Modifier
            .fillMaxWidth()
            .clickable { openMap(lat, lng) }
            .background(Color(0xFFFFCDD2))
    ) {
        Column(
            modifier = Modifier
                .padding(16.dp)
                .fillMaxWidth(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "⚠️ Fall Detected!",
                fontSize = 20.sp,
                fontWeight = FontWeight.Bold,
                color = Color.Red
            )
            Spacer(modifier = Modifier.height(8.dp))
            Text(
                text = "Click to view location on the map",
                fontSize = 16.sp,
                color = Color.Blue
            )
        }
    }
}

@Preview(showBackground = true)
@Composable
fun MainDashboardPreview() {
    Smart_WheelchairTheme {
        MainDashboard(FirebaseDatabase.getInstance().reference) { _, _ -> }
    }
}
