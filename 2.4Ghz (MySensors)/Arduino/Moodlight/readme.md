### Buttons
```json
{
      "id": "moodlight",
      "name": "Mood Light",
      "class": "ButtonsDevice",
      "buttons": [
        {
          "id": "mood-red",
          "text": "Red"
        }
      ]
    },
```

### Rules

```json
{
      "id": "moodlight-red",
      "name": "Moodlight red",
      "rule": "if mood-red is pressed then send Ir nodeid:\"50\" sensorid:\"0\" cmdcode:\"ff0000\"",
      "active": true,
      "logging": false
    },
```