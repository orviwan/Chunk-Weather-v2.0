module.exports = [
  {
    "type": "heading",
    "defaultValue": "Chunk Weather v4.0"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "TopBackgroundColor",
        "defaultValue": "0x000000",
        "label": "Top Background Color"
      },
      {
        "type": "color",
        "messageKey": "TopForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Top Foreground Color"
      },
      {
        "type": "color",
        "messageKey": "BottomBackgroundColor",
        "defaultValue": "0x000000",
        "label": "Bottom Background Color"
      },
      {
        "type": "color",
        "messageKey": "BottomForegroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Bottom Foreground Color"
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "More Settings"
      },
      {
        "type": "toggle",
        "messageKey": "BluetoothVibe",
        "label": "Vibrate on Bluetooth Disconnect/Connect",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "HourlyVibe",
        "label": "Enable Vibrations on the Hour",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "Blink",
        "label": "Enable Blinking Time Separator",
        "defaultValue": false
      },
      {
        "type": "toggle",
        "messageKey": "DateFormat",
        "label": "Show Date Suffix",
        "defaultValue": true
      },
      {
        "type": "toggle",
        "messageKey": "WeatherIsFahrenheit",
        "label": "Weather is Fahrenheit",
        "defaultValue": true
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
