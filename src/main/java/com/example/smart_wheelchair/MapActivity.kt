package com.example.smart_wheelchair

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import org.osmdroid.config.Configuration
import org.osmdroid.views.MapView
import org.osmdroid.views.overlay.Marker

class MapActivity : AppCompatActivity() {

    private lateinit var map: MapView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Configuration.getInstance().load(this, getSharedPreferences("osmdroid", MODE_PRIVATE))
        map = MapView(this)
        setContentView(map)

        // Hardcoded latitude and longitude
        val latitude = 10.904048600061039
        val longitude = 76.8983838821322

        setupMap(latitude, longitude)
    }

    private fun setupMap(lat: Double, lng: Double) {
        val mapController = map.controller
        mapController.setZoom(18.0)
        mapController.setCenter(org.osmdroid.util.GeoPoint(lat, lng))

        val marker = Marker(map)
        marker.position = org.osmdroid.util.GeoPoint(lat, lng)
        marker.title = "Fall Detected Location"
        marker.setAnchor(Marker.ANCHOR_CENTER, Marker.ANCHOR_BOTTOM)

        map.overlays.add(marker)
    }

    override fun onResume() {
        super.onResume()
        map.onResume()
    }

    override fun onPause() {
        super.onPause()
        map.onPause()
    }
}
