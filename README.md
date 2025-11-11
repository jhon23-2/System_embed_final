# 🌡️ Sistema de Monitoreo de Temperatura y Humedad con DHT11

Proyecto de sistema embebido para monitoreo ambiental con capacidades de almacenamiento histórico, análisis estadístico y pronóstico básico.

## 📋 Descripción

Sistema de adquisición y análisis de datos ambientales basado en microcontrolador PIC16F887 que:

- Mide temperatura (0-50°C) y humedad relativa (20-80%)
- Almacena historial de lecturas en EEPROM interna
- Calcula estadísticas básicas (mínimo, máximo, promedio)
- Genera pronósticos simples basados en promedios móviles
- Proporciona indicadores visuales mediante LEDs
- Muestra datos en tiempo real en pantalla LCD 16x2

## 🔧 Hardware Requerido

### Componentes Principales

- **Microcontrolador**: PIC16F887
- **Sensor**: DHT11 (temperatura y humedad)
- **Display**: LCD 16x2 con adaptador I2C (PCF8574)
- **Cristal**: 20MHz
- **LEDs**: 6 unidades (indicadores de estado)
- **Resistencias**:
  - 1x 10kΩ (pull-up DHT11)
  - 6x 330Ω (LEDs)
  - 2x 10kΩ (cristal)
- **Capacitores**: 2x 22pF (cristal)

### Diagrama de Conexiones

```
PIC16F887 Pinout:
┌─────────────────────────────────┐
│  RB0 ──────► DHT11 (Data)       │
│  RB1 ──────► I2C SDA (LCD)      │
│  RB2 ──────► I2C SCL (LCD)      │
│  RD0 ──────► LED Frío (Azul)    │
│  RD1 ──────► LED Normal (Verde) │
│  RD2 ──────► LED Calor (Rojo)   │
│  RD3 ──────► LED Seco (Amarillo)│
│  RD4 ──────► LED Húmedo (Cyan)  │
│  RD5 ──────► LED Tendencia      │
└─────────────────────────────────┘

DHT11 Conexión:
Pin 1 (VCC)  → +5V
Pin 2 (DATA) → RB0 + Resistencia 10kΩ a +5V
Pin 3 (NC)   → No conectar
Pin 4 (GND)  → GND
```

## 💾 Características del Software

### Almacenamiento de Datos

- **EEPROM Interna**: 256 bytes disponibles
- **Capacidad**: Hasta 30 lecturas históricas
- **Formato**: 2 bytes por lectura (1 byte temp + 1 byte humedad)
- **Método**: Buffer circular (sobrescribe datos más antiguos)

### Funciones de Análisis

#### 1. Estadísticas Básicas

```c
- Temperatura mínima y máxima
- Humedad mínima y máxima
- Promedio móvil de últimas N lecturas
- Contador de lecturas totales
```

#### 2. Detección de Tendencias

```c
- Compara promedio de últimas 3 lecturas vs 3 anteriores
- Detecta si temperatura está subiendo (↑), bajando (↓) o estable (-)
- Threshold de ±1°C para considerar cambio significativo
```

#### 3. Pronóstico Simple

```c
- Basado en promedio de últimas 5 lecturas
- Genera predicción de temperatura y humedad
- Actualizado cada vez que se guarda nueva lectura
```

### Indicadores LED

| LED            | Color    | Condición               | Pin |
| -------------- | -------- | ----------------------- | --- |
| LED_FRIO       | Azul     | Temp < 20°C             | RD0 |
| LED_NORMAL     | Verde    | 20°C ≤ Temp ≤ 28°C      | RD1 |
| LED_CALOR      | Rojo     | Temp > 28°C             | RD2 |
| LED_SECO       | Amarillo | Humedad < 40%           | RD3 |
| LED_HUMEDO     | Cyan     | Humedad > 70%           | RD4 |
| LED_PRONOSTICO | Magenta  | Tendencia fuerte (±2°C) | RD5 |

### Modos de Visualización LCD

El sistema rota automáticamente entre 3 modos cada 8 segundos:

**Modo 0 - Vista Actual**

```
T:25C  H:65%
Mem:15 Tend:^
```

**Modo 1 - Pronóstico**

```
PRONOSTICO:
T:26C  H:64%
```

**Modo 2 - Estadísticas**

```
T:18-32C
H:45-80%
```

## 🚀 Instalación y Uso

### Requisitos de Software

- MPLAB X IDE v5.0 o superior
- XC8 Compiler v2.0 o superior
- Proteus 8.x (para simulación)

### Compilación

1. **Clonar el repositorio**

```bash
git clone https://github.com/tu-usuario/dht11-pic16f887.git
cd dht11-pic16f887
```

2. **Abrir proyecto en MPLAB X**

```
File → Open Project → Seleccionar carpeta del proyecto
```

3. **Compilar**

```
Production → Build Main Project (F11)
```

4. **Generar HEX**
   El archivo `.hex` se genera en `dist/default/production/`

### Configuración Inicial

#### Ajustar Frecuencia de Guardado

Para cambiar la frecuencia de almacenamiento en EEPROM:

```c
// En main.c, línea ~280
if(contador_muestras >= 10) {  // Cambiar este valor
    guardar_lectura(tem, hum);
    // ...
}
```

**Valores recomendados:**

- `10` = ~20 segundos (pruebas rápidas) ✅ Valor actual
- `30` = 1 minuto
- `900` = 30 minutos
- `1800` = 1 hora ⭐ Recomendado para uso real

#### Ajustar Umbrales de LEDs

Modificar en `main.c` líneas 220-230:

```c
LED_FRIO = (temp < 20) ? 1 : 0;     // Cambiar 20
LED_NORMAL = (temp >= 20 && temp <= 28) ? 1 : 0;  // Cambiar 28
LED_CALOR = (temp > 28) ? 1 : 0;    // Cambiar 28
LED_SECO = (hum < 40) ? 1 : 0;      // Cambiar 40
LED_HUMEDO = (hum > 70) ? 1 : 0;    // Cambiar 70
```

## 📊 Estructura del Proyecto

```
dht11-pic16f887/
├── main.c                 # Programa principal
├── i2c.h                  # Librería I2C
├── i2c.c
├── lcd_i2c.h              # Librería LCD I2C
├── lcd_i2c.c
├── README.md              # Este archivo
├── docs/
│   ├── schematic.pdf      # Esquemático del circuito
│   ├── datasheet_dht11.pdf
│   └── datasheet_pic16f887.pdf
├── simulation/
│   └── proteus_project.pdsprj
└── images/
    ├── circuit_diagram.png
    └── lcd_display.png
```

## 🔬 Protocolo de Comunicación DHT11

### Secuencia de Lectura

1. **Inicio**: MCU envía señal LOW por 18ms
2. **Respuesta**: DHT11 responde con señal LOW 80µs + HIGH 80µs
3. **Transmisión**: 40 bits de datos (5 bytes)
   - Byte 0: Humedad parte entera
   - Byte 1: Humedad parte decimal (siempre 0 en DHT11)
   - Byte 2: Temperatura parte entera
   - Byte 3: Temperatura parte decimal (siempre 0 en DHT11)
   - Byte 4: Checksum (suma de bytes 0-3)

### Timing de Bits

- **Bit '0'**: 50µs LOW + 26-28µs HIGH
- **Bit '1'**: 50µs LOW + 70µs HIGH

## 📈 Algoritmos Implementados

### Cálculo de Promedio Móvil

```c
promedio = Σ(últimas_n_lecturas) / n
```

### Detección de Tendencia

```c
tendencia = promedio(últimas_3) - promedio(3_anteriores)

Si tendencia > 1.0°C  → ↑ Subiendo
Si tendencia < -1.0°C → ↓ Bajando
Sino                  → - Estable
```

### Pronóstico Simple

```c
pronóstico = promedio(últimas_5_lecturas)
```

## 🐛 Solución de Problemas

### Error: "Error DHT11 - Check conexion"

- Verificar conexión del pin DATA (RB0)
- Confirmar resistencia pull-up de 10kΩ
- Revisar alimentación del DHT11 (+5V estable)

### LCD no muestra nada

- Verificar conexiones I2C (SDA=RB1, SCL=RB2)
- Confirmar dirección I2C del módulo (generalmente 0x27 o 0x3F)
- Ajustar potenciómetro de contraste en el módulo I2C

### LEDs no encienden

- Verificar configuración de TRISD (debe ser 0x00)
- Confirmar resistencias de 330Ω en serie
- Revisar que PORTD esté correctamente configurado

### Lecturas erráticas

- Aumentar tiempo entre lecturas (mínimo 2 segundos)
- Verificar checksum en la función `DHT11_Read()`
- Evitar cables largos (máximo 20cm recomendado)

## 📝 Especificaciones Técnicas

### DHT11

- Rango Temperatura: 0-50°C (±2°C)
- Rango Humedad: 20-80% (±5%)
- Resolución: 1°C / 1%
- Tiempo de respuesta: 6-15 segundos
- Frecuencia de muestreo: 1 Hz (1 lectura/segundo)

### PIC16F887

- Arquitectura: 8-bit RISC
- Frecuencia: 20 MHz
- Memoria Flash: 8K palabras
- EEPROM: 256 bytes
- RAM: 368 bytes
- Pines I/O: 35

## 🤝 Contribuciones

Las contribuciones son bienvenidas. Por favor:

1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -m 'Agrega nueva funcionalidad'`)
4. Push a la rama (`git push origin feature/nueva-funcionalidad`)
5. Abre un Pull Request

## 📄 Licencia

Este proyecto está bajo la Licencia MIT - ver el archivo [LICENSE](LICENSE) para más detalles.

## ✨ Autor

**Tu Nombre**

- GitHub: [@jhon23-2](https://github.com/jhon23-2)
- Email: tu-email@ejemplo.com

## 🙏 Agradecimientos

- Microchip Technology por la documentación del PIC16F887
- Comunidad de Arduino/PIC por librerías de referencia
- Profesor [Nombre del Profesor] por el diseño del proyecto

## 📚 Referencias

1. [PIC16F887 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/41291D.pdf)
2. [DHT11 Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
3. [PCF8574 I2C LCD Adapter](https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf)

---

⭐ Si este proyecto te fue útil, considera darle una estrella en GitHub!
