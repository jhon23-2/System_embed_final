"""
Termómetro Digital con Análisis Estadístico
=============================================
Análisis de datos de temperatura y humedad del PIC16F887 + DHT11

Autor: Tu Nombre
Fecha: 29/10/2025
Microcontrolador: PIC16F887
Sensores: DHT11, DS1307
"""

# ============================================================================
# 1. IMPORTAR LIBRERÍAS
# ============================================================================

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from datetime import datetime, timedelta
from scipy import stats
from statsmodels.tsa.holtwinters import SimpleExpSmoothing
import warnings
warnings.filterwarnings('ignore')

# Configuración de gráficos
plt.style.use('seaborn-v0_8-darkgrid')
sns.set_palette("husl")

print("=" * 60)
print("  TERMÓMETRO DIGITAL - ANÁLISIS ESTADÍSTICO")
print("=" * 60)
print()

# ============================================================================
# 2. GENERAR DATOS DE EJEMPLO (Reemplazar con lectura de SD)
# ============================================================================

def generar_datos_ejemplo(num_horas=24):
    """
    Genera datos de ejemplo simulando lecturas del DHT11
    En producción, estos datos vendrían del archivo CSV de la SD
    """
    np.random.seed(42)
    fechas = [datetime.now() - timedelta(hours=i) for i in range(num_horas-1, -1, -1)]
    
    # Simular temperatura con patrón diurno
    horas = np.arange(num_horas)
    temp_base = 25 + 5 * np.sin(2 * np.pi * horas / 24)  # Ciclo diurno
    temperatura = temp_base + np.random.normal(0, 1, num_horas)
    
    # Simular humedad inversamente proporcional a temperatura
    hum_base = 70 - 10 * np.sin(2 * np.pi * horas / 24)
    humedad = hum_base + np.random.normal(0, 3, num_horas)
    
    df = pd.DataFrame({
        'fecha_hora': fechas,
        'temperatura': temperatura,
        'humedad': humedad
    })
    
    return df

# Generar datos de ejemplo
df = generar_datos_ejemplo(24)

print(f"📊 Datos cargados: {len(df)} registros")
print(f"📅 Rango de fechas: {df['fecha_hora'].min()} a {df['fecha_hora'].max()}")
print()

# ============================================================================
# 3. FUNCIÓN PARA LEER DATOS DESDE SD CARD
# ============================================================================

def leer_datos_sd(archivo='datos_sensor.csv'):
    """
    Lee los datos guardados por el microcontrolador en la SD
    
    Formato esperado del CSV:
    fecha_hora,temperatura,humedad
    2025-10-29 10:00:00,25.5,65.2
    2025-10-29 11:00:00,26.1,64.8
    ...
    """
    try:
        df = pd.read_csv(archivo)
        df['fecha_hora'] = pd.to_datetime(df['fecha_hora'])
        print(f"✅ Archivo '{archivo}' leído exitosamente")
        return df
    except FileNotFoundError:
        print(f"⚠️  Archivo '{archivo}' no encontrado. Usando datos de ejemplo.")
        return generar_datos_ejemplo(24)
    except Exception as e:
        print(f"❌ Error al leer archivo: {e}")
        return None

# Descomentar para leer datos reales de SD:
# df = leer_datos_sd('datos_sensor.csv')

# ============================================================================
# 4. RESUMEN ESTADÍSTICO
# ============================================================================

print("=" * 60)
print("  RESUMEN ESTADÍSTICO")
print("=" * 60)
print()

def calcular_estadisticas(serie, nombre):
    """
    Calcula todas las estadísticas requeridas:
    Count, Mean, Std, Min, 25%, 50%, 75%, Max
    """
    estadisticas = {
        'Count': len(serie),
        'Mean': np.mean(serie),
        'Std': np.std(serie, ddof=1),  # Desviación estándar muestral
        'Min': np.min(serie),
        '25%': np.percentile(serie, 25),
        '50%': np.percentile(serie, 50),  # Mediana
        '75%': np.percentile(serie, 75),
        'Max': np.max(serie)
    }
    
    print(f"📈 {nombre}")
    print("-" * 60)
    for stat, valor in estadisticas.items():
        print(f"  {stat:8s}: {valor:8.2f}")
    print()
    
    return estadisticas

# Calcular estadísticas
stats_temp = calcular_estadisticas(df['temperatura'], 'TEMPERATURA (°C)')
stats_hum = calcular_estadisticas(df['humedad'], 'HUMEDAD (%)')

# Crear DataFrame con estadísticas
df_stats = pd.DataFrame({
    'Temperatura': stats_temp,
    'Humedad': stats_hum
})

print("📊 Tabla Comparativa:")
print(df_stats.round(2))
print()

# ============================================================================
# 5. VISUALIZACIÓN DE DATOS
# ============================================================================

print("=" * 60)
print("  VISUALIZACIÓN DE DATOS")
print("=" * 60)
print()

fig, axes = plt.subplots(2, 2, figsize=(14, 10))
fig.suptitle('Análisis de Temperatura y Humedad', fontsize=16, fontweight='bold')

# Gráfico 1: Serie temporal de temperatura
axes[0, 0].plot(df['fecha_hora'], df['temperatura'], marker='o', color='orangered', linewidth=2)
axes[0, 0].set_title('Temperatura vs Tiempo')
axes[0, 0].set_xlabel('Fecha y Hora')
axes[0, 0].set_ylabel('Temperatura (°C)')
axes[0, 0].grid(True, alpha=0.3)
axes[0, 0].tick_params(axis='x', rotation=45)

# Gráfico 2: Serie temporal de humedad
axes[0, 1].plot(df['fecha_hora'], df['humedad'], marker='s', color='dodgerblue', linewidth=2)
axes[0, 1].set_title('Humedad vs Tiempo')
axes[0, 1].set_xlabel('Fecha y Hora')
axes[0, 1].set_ylabel('Humedad (%)')
axes[0, 1].grid(True, alpha=0.3)
axes[0, 1].tick_params(axis='x', rotation=45)

# Gráfico 3: Histograma de temperatura
axes[1, 0].hist(df['temperatura'], bins=15, color='orangered', alpha=0.7, edgecolor='black')
axes[1, 0].axvline(stats_temp['Mean'], color='red', linestyle='--', linewidth=2, label='Media')
axes[1, 0].set_title('Distribución de Temperatura')
axes[1, 0].set_xlabel('Temperatura (°C)')
axes[1, 0].set_ylabel('Frecuencia')
axes[1, 0].legend()
axes[1, 0].grid(True, alpha=0.3)

# Gráfico 4: Histograma de humedad
axes[1, 1].hist(df['humedad'], bins=15, color='dodgerblue', alpha=0.7, edgecolor='black')
axes[1, 1].axvline(stats_hum['Mean'], color='blue', linestyle='--', linewidth=2, label='Media')
axes[1, 1].set_title('Distribución de Humedad')
axes[1, 1].set_xlabel('Humedad (%)')
axes[1, 1].set_ylabel('Frecuencia')
axes[1, 1].legend()
axes[1, 1].grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('analisis_datos.png', dpi=300, bbox_inches='tight')
print("✅ Gráficos guardados en 'analisis_datos.png'")
plt.show()

# ============================================================================
# 6. PRONÓSTICO DE 6 PASOS (6 HORAS)
# ============================================================================

print()
print("=" * 60)
print("  PRONÓSTICO - 6 HORAS HACIA EL FUTURO")
print("=" * 60)
print()

def pronostico_media_movil(datos, ventana=3, pasos=6):
    """
    Pronóstico usando Media Móvil Simple
    Este método es simple y puede implementarse en el microcontrolador
    """
    pronosticos = []
    datos_actualizados = list(datos)
    
    for _ in range(pasos):
        # Calcular promedio de los últimos 'ventana' valores
        promedio = np.mean(datos_actualizados[-ventana:])
        pronosticos.append(promedio)
        datos_actualizados.append(promedio)
    
    return np.array(pronosticos)

def pronostico_suavizamiento_exponencial(datos, pasos=6):
    """
    Pronóstico usando Suavizamiento Exponencial Simple
    Método más sofisticado para Python
    """
    modelo = SimpleExpSmoothing(datos)
    ajuste = modelo.fit()
    pronostico = ajuste.forecast(steps=pasos)
    return pronostico

# Generar pronósticos
temp_actual = df['temperatura'].values
hum_actual = df['humedad'].values

# Método 1: Media Móvil (para microcontrolador)
forecast_temp_ma = pronostico_media_movil(temp_actual, ventana=3, pasos=6)
forecast_hum_ma = pronostico_media_movil(hum_actual, ventana=3, pasos=6)

# Método 2: Suavizamiento Exponencial (para Python)
forecast_temp_es = pronostico_suavizamiento_exponencial(temp_actual, pasos=6)
forecast_hum_es = pronostico_suavizamiento_exponencial(hum_actual, pasos=6)

# Crear fechas futuras
ultima_fecha = df['fecha_hora'].iloc[-1]
fechas_futuras = [ultima_fecha + timedelta(hours=i+1) for i in range(6)]

# Mostrar pronósticos
print("🔮 PRONÓSTICO - MÉTODO 1: MEDIA MÓVIL (Microcontrolador)")
print("-" * 60)
print(f"{'Hora':<20} {'Temperatura (°C)':>18} {'Humedad (%)':>18}")
print("-" * 60)
for i, fecha in enumerate(fechas_futuras):
    print(f"{fecha.strftime('%Y-%m-%d %H:%M'):<20} {forecast_temp_ma[i]:>18.2f} {forecast_hum_ma[i]:>18.2f}")
print()

print("🔮 PRONÓSTICO - MÉTODO 2: SUAVIZAMIENTO EXPONENCIAL (Python)")
print("-" * 60)
print(f"{'Hora':<20} {'Temperatura (°C)':>18} {'Humedad (%)':>18}")
print("-" * 60)
for i, fecha in enumerate(fechas_futuras):
    print(f"{fecha.strftime('%Y-%m-%d %H:%M'):<20} {forecast_temp_es[i]:>18.2f} {forecast_hum_es[i]:>18.2f}")
print()

# Visualizar pronósticos
fig, axes = plt.subplots(1, 2, figsize=(14, 5))
fig.suptitle('Pronóstico de 6 Horas', fontsize=16, fontweight='bold')

# Pronóstico de temperatura
axes[0].plot(df['fecha_hora'], df['temperatura'], 'o-', label='Datos Históricos', color='orangered', linewidth=2)
axes[0].plot(fechas_futuras, forecast_temp_ma, 's--', label='Media Móvil', color='darkred', linewidth=2)
axes[0].plot(fechas_futuras, forecast_temp_es, '^--', label='Suav. Exponencial', color='coral', linewidth=2)
axes[0].set_title('Pronóstico de Temperatura')
axes[0].set_xlabel('Fecha y Hora')
axes[0].set_ylabel('Temperatura (°C)')
axes[0].legend()
axes[0].grid(True, alpha=0.3)
axes[0].tick_params(axis='x', rotation=45)

# Pronóstico de humedad
axes[1].plot(df['fecha_hora'], df['humedad'], 'o-', label='Datos Históricos', color='dodgerblue', linewidth=2)
axes[1].plot(fechas_futuras, forecast_hum_ma, 's--', label='Media Móvil', color='darkblue', linewidth=2)
axes[1].plot(fechas_futuras, forecast_hum_es, '^--', label='Suav. Exponencial', color='skyblue', linewidth=2)
axes[1].set_title('Pronóstico de Humedad')
axes[1].set_xlabel('Fecha y Hora')
axes[1].set_ylabel('Humedad (%)')
axes[1].legend()
axes[1].grid(True, alpha=0.3)
axes[1].tick_params(axis='x', rotation=45)

plt.tight_layout()
plt.savefig('pronostico_6horas.png', dpi=300, bbox_inches='tight')
print("✅ Gráfico de pronóstico guardado en 'pronostico_6horas.png'")
plt.show()

# ============================================================================
# 7. EXPORTAR RESULTADOS
# ============================================================================

print()
print("=" * 60)
print("  EXPORTACIÓN DE RESULTADOS")
print("=" * 60)
print()

# Crear DataFrame con pronósticos
df_forecast = pd.DataFrame({
    'fecha_hora': fechas_futuras,
    'temp_media_movil': forecast_temp_ma,
    'hum_media_movil': forecast_hum_ma,
    'temp_exp_smoothing': forecast_temp_es,
    'hum_exp_smoothing': forecast_hum_es
})

# Guardar estadísticas
df_stats.to_csv('estadisticas_resumidas.csv')
print("✅ Estadísticas guardadas en 'estadisticas_resumidas.csv'")

# Guardar pronósticos
df_forecast.to_csv('pronosticos_6horas.csv', index=False)
print("✅ Pronósticos guardados en 'pronosticos_6horas.csv'")

print()
print("=" * 60)
print("  ANÁLISIS COMPLETADO")
print("=" * 60)
print()
print("📁 Archivos generados:")
print("  - analisis_datos.png")
print("  - pronostico_6horas.png")
print("  - estadisticas_resumidas.csv")
print("  - pronosticos_6horas.csv")
print()
print("🎉 ¡Análisis finalizado exitosamente!")
