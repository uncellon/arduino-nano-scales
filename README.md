# ATmega328P weighing device

## Description

This firmware makes it possible to make a weighing device using 4 HX711 ADCs.

Main features:
- Get average weight
- Weight reading from each ADC (faster)
- Taring
- Calibration with saving values in EEPROM

## Examples

### Average weight request

Request:
```
AWEND
```

Response:
```
AW-115.45END
```

### Weight request

Request:
```
WEND
```

Response:
```
WF11.68S-4183.17T2856.54F-2606.26
```
where 11.68 from first load cell, -4183.17 from second load cell, 2856.54 from third load cell and -2606.26 from fourth cell

### Taring request

Request:
```
TAREEND
```

Response:
```
TAREEND
```

### Get calibration factor

A typical request looks like ```GETCAL$1END```, where ```$1``` is load cell number (from 1 to 4).

Request:
```
GETCAL1END
```

Response:
```
GETCAL-3END
```

Error:
```
GETCALERREND
```

### Set calibration factor

A typical request looks like ```GETCAL$1$2END```, where ```$1``` is load cell number (from 1 to 4), ```$2``` is calibration factor.

Request:
```
SETCAL1-115END
```

Response:
```
SETCAL1END
```

Error:
```
SETCALERREND
```