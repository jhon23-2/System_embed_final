import React, { useState } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { Download, TrendingUp, BarChart3, Clock } from 'lucide-react';

const TermometroAnalysis = () => {
  const [activeTab, setActiveTab] = useState('intro');

  // Datos de ejemplo simulados
  const generateSampleData = () => {
    const data = [];
    const baseTemp = 25;
    const baseHum = 60;
    for (let i = 0; i < 24; i++) {
      data.push({
        hora: `${i}:00`,
        temperatura: baseTemp + Math.sin(i / 3) * 3 + Math.random() * 2,
        humedad: baseHum + Math.cos(i / 4) * 10 + Math.random() * 5
      });
    }
    return data;
  };

  const sampleData = generateSampleData();

  // Estadísticas calculadas
  const calcStats = (values) => {
    const sorted = [...values].sort((a, b) => a - b);
    const n = sorted.length;
    return {
      count: n,
      mean: (values.reduce((a, b) => a + b, 0) / n).toFixed(2),
      std: Math.sqrt(values.reduce((sum, val) => sum + Math.pow(val - values.reduce((a, b) => a + b, 0) / n, 2), 0) / n).toFixed(2),
      min: Math.min(...values).toFixed(2),
      q25: sorted[Math.floor(n * 0.25)].toFixed(2),
      q50: sorted[Math.floor(n * 0.50)].toFixed(2),
      q75: sorted[Math.floor(n * 0.75)].toFixed(2),
      max: Math.max(...values).toFixed(2)
    };
  };

  const tempStats = calcStats(sampleData.map(d => d.temperatura));
  const humStats = calcStats(sampleData.map(d => d.humedad));

  const renderIntro = () => (
    <div className="space-y-6">
      <div className="bg-gradient-to-r from-blue-50 to-indigo-50 p-6 rounded-lg border border-blue-200">
        <h2 className="text-2xl font-bold text-blue-900 mb-4 flex items-center gap-2">
          <BarChart3 className="w-6 h-6" />
          Termómetro Digital con Análisis Estadístico
        </h2>
        <p className="text-gray-700 leading-relaxed">
          Este notebook demuestra el análisis estadístico y pronóstico de datos de temperatura y humedad,
          sirviendo como referencia para la implementación en el microcontrolador PIC16F887.
        </p>
      </div>

      <div className="grid md:grid-cols-3 gap-4">
        <div className="bg-white p-4 rounded-lg shadow border-l-4 border-blue-500">
          <div className="flex items-center gap-2 mb-2">
            <Clock className="w-5 h-5 text-blue-600" />
            <h3 className="font-semibold text-gray-800">Tiempo Real</h3>
          </div>
          <p className="text-sm text-gray-600">RTC DS1307 para marcas temporales precisas</p>
        </div>

        <div className="bg-white p-4 rounded-lg shadow border-l-4 border-green-500">
          <div className="flex items-center gap-2 mb-2">
            <TrendingUp className="w-5 h-5 text-green-600" />
            <h3 className="font-semibold text-gray-800">Pronóstico</h3>
          </div>
          <p className="text-sm text-gray-600">Predicción de 6 horas usando media móvil</p>
        </div>

        <div className="bg-white p-4 rounded-lg shadow border-l-4 border-purple-500">
          <div className="flex items-center gap-2 mb-2">
            <Download className="w-5 h-5 text-purple-600" />
            <h3 className="font-semibold text-gray-800">Almacenamiento</h3>
          </div>
          <p className="text-sm text-gray-600">Datos guardados en tarjeta SD</p>
        </div>
      </div>

      <div className="bg-white p-6 rounded-lg shadow">
        <h3 className="text-lg font-semibold mb-4 text-gray-800">Librerías Requeridas</h3>
        <pre className="bg-gray-900 text-green-400 p-4 rounded overflow-x-auto">
{`# Instalar librerías necesarias
pip install pandas numpy matplotlib seaborn scipy statsmodels`}
        </pre>
      </div>
    </div>
  );

  const renderStatistics = () => (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-gray-800 mb-4">Resumen Estadístico</h2>
      
      <div className="grid md:grid-cols-2 gap-6">
        <div className="bg-white p-6 rounded-lg shadow">
          <h3 className="text-lg font-semibold mb-4 text-orange-600 flex items-center gap-2">
            🌡️ Temperatura (°C)
          </h3>
          <table className="w-full text-sm">
            <tbody>
              <tr className="border-b"><td className="py-2 font-medium">Count</td><td className="text-right">{tempStats.count}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Mean</td><td className="text-right">{tempStats.mean}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Std</td><td className="text-right">{tempStats.std}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Min</td><td className="text-right">{tempStats.min}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">25%</td><td className="text-right">{tempStats.q25}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">50%</td><td className="text-right">{tempStats.q50}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">75%</td><td className="text-right">{tempStats.q75}</td></tr>
              <tr><td className="py-2 font-medium">Max</td><td className="text-right">{tempStats.max}</td></tr>
            </tbody>
          </table>
        </div>

        <div className="bg-white p-6 rounded-lg shadow">
          <h3 className="text-lg font-semibold mb-4 text-blue-600 flex items-center gap-2">
            💧 Humedad (%)
          </h3>
          <table className="w-full text-sm">
            <tbody>
              <tr className="border-b"><td className="py-2 font-medium">Count</td><td className="text-right">{humStats.count}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Mean</td><td className="text-right">{humStats.mean}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Std</td><td className="text-right">{humStats.std}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">Min</td><td className="text-right">{humStats.min}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">25%</td><td className="text-right">{humStats.q25}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">50%</td><td className="text-right">{humStats.q50}</td></tr>
              <tr className="border-b"><td className="py-2 font-medium">75%</td><td className="text-right">{humStats.q75}</td></tr>
              <tr><td className="py-2 font-medium">Max</td><td className="text-right">{humStats.max}</td></tr>
            </tbody>
          </table>
        </div>
      </div>

      <div className="bg-white p-6 rounded-lg shadow">
        <h3 className="text-lg font-semibold mb-4 text-gray-800">Código Python</h3>
        <pre className="bg-gray-900 text-green-400 p-4 rounded overflow-x-auto text-sm">
{`import pandas as pd
import numpy as np

# Leer datos desde CSV
df = pd.read_csv('datos_sensor.csv')

# Calcular estadísticas descriptivas
stats_temp = df['temperatura'].describe()
stats_hum = df['humedad'].describe()

print("=== Estadísticas de Temperatura ===")
print(stats_temp)
print("\\n=== Estadísticas de Humedad ===")
print(stats_hum)`}
        </pre>
      </div>
    </div>
  );

  const renderForecast = () => (
    <div className="space-y-6">
      <h2 className="text-2xl font-bold text-gray-800 mb-4">Pronóstico (6 horas)</h2>
      
      <ResponsiveContainer width="100%" height={300}>
        <LineChart data={sampleData}>
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="hora" />
          <YAxis />
          <Tooltip />
          <Legend />
          <Line type="monotone" dataKey="temperatura" stroke="#f97316" strokeWidth={2} name="Temperatura (°C)" />
          <Line type="monotone" dataKey="humedad" stroke="#3b82f6" strokeWidth={2} name="Humedad (%)" />
        </LineChart>
      </ResponsiveContainer>

      <div className="bg-white p-6 rounded-lg shadow">
        <h3 className="text-lg font-semibold mb-4 text-gray-800">Métodos de Pronóstico</h3>
        <div className="space-y-4">
          <div className="border-l-4 border-blue-500 pl-4">
            <h4 className="font-semibold text-blue-900">1. Media Móvil Simple</h4>
            <p className="text-sm text-gray-600">Promedio de las últimas N observaciones</p>
          </div>
          <div className="border-l-4 border-green-500 pl-4">
            <h4 className="font-semibold text-green-900">2. Suavizamiento Exponencial</h4>
            <p className="text-sm text-gray-600">Pesos decrecientes para datos antiguos</p>
          </div>
          <div className="border-l-4 border-purple-500 pl-4">
            <h4 className="font-semibold text-purple-900">3. ARIMA (Avanzado)</h4>
            <p className="text-sm text-gray-600">Modelo autorregresivo integrado de media móvil</p>
          </div>
        </div>
      </div>

      <div className="bg-white p-6 rounded-lg shadow">
        <h3 className="text-lg font-semibold mb-4 text-gray-800">Código de Pronóstico</h3>
        <pre className="bg-gray-900 text-green-400 p-4 rounded overflow-x-auto text-sm">
{`from statsmodels.tsa.holtwinters import SimpleExpSmoothing
import numpy as np

# Método 1: Media Móvil Simple (para microcontrolador)
def moving_average_forecast(data, window=3, steps=6):
    forecasts = []
    for i in range(steps):
        avg = np.mean(data[-window:])
        forecasts.append(avg)
        data = np.append(data, avg)
    return forecasts

# Método 2: Suavizamiento Exponencial (Python)
def exponential_smoothing_forecast(data, steps=6):
    model = SimpleExpSmoothing(data)
    fit = model.fit()
    forecast = fit.forecast(steps=steps)
    return forecast

# Ejemplo de uso
temp_data = df['temperatura'].values
forecast_temp = moving_average_forecast(temp_data, window=3, steps=6)
print(f"Pronóstico 6 horas: {forecast_temp}")`}
        </pre>
      </div>
    </div>
  );

  const tabs = [
    { id: 'intro', label: 'Introducción', icon: '📚' },
    { id: 'stats', label: 'Estadísticas', icon: '📊' },
    { id: 'forecast', label: 'Pronóstico', icon: '📈' }
  ];

  return (
    <div className="min-h-screen bg-gradient-to-br from-gray-50 to-blue-50 p-6">
      <div className="max-w-6xl mx-auto">
        <div className="bg-white rounded-lg shadow-lg overflow-hidden mb-6">
          <div className="bg-gradient-to-r from-blue-600 to-indigo-600 p-6 text-white">
            <h1 className="text-3xl font-bold mb-2">Análisis de Datos - Termómetro Digital</h1>
            <p className="text-blue-100">PIC16F887 + DHT11 + DS1307 + SD Card</p>
          </div>

          <div className="flex border-b bg-gray-50">
            {tabs.map(tab => (
              <button
                key={tab.id}
                onClick={() => setActiveTab(tab.id)}
                className={`flex-1 py-3 px-4 font-medium transition-colors ${
                  activeTab === tab.id
                    ? 'bg-white text-blue-600 border-b-2 border-blue-600'
                    : 'text-gray-600 hover:bg-gray-100'
                }`}
              >
                <span className="mr-2">{tab.icon}</span>
                {tab.label}
              </button>
            ))}
          </div>

          <div className="p-6">
            {activeTab === 'intro' && renderIntro()}
            {activeTab === 'stats' && renderStatistics()}
            {activeTab === 'forecast' && renderForecast()}
          </div>
        </div>

        <div className="bg-yellow-50 border-l-4 border-yellow-400 p-4 rounded">
          <p className="text-sm text-yellow-800">
            <strong>Nota:</strong> Este notebook es una demostración interactiva. 
            Descarga el archivo .ipynb completo desde el repositorio para ejecutar el código real.
          </p>
        </div>
      </div>
    </div>
  );
};

export default TermometroAnalysis;
