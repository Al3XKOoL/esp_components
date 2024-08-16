#include "custom_climate.h"

namespace esphome {
namespace custom_climate {

void CustomClimate::setup() {
  // Inicialización si es necesario
}

void CustomClimate::loop() {
  unsigned long current_time = millis();

  if (current_time - last_check_time_ >= interval_seconds_ * 1000) {
    last_check_time_ = current_time;
    check_and_control_pump();
  }
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
    this->publish_state();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
    this->publish_state();
  }
}
// TESTING
void CustomClimate::update_state() {
  // Configura el estado de tu climatizador
  this->current_temperature = this->temperature_spa_sensor_->state;
  this->target_temperature = this->get_target_temperature();  // Suponiendo que tienes un método para obtener la temperatura objetivo
  this->mode = climate::CLIMATE_MODE_HEAT;  // Por ejemplo, modo calefacción
  this->action = climate::CLIMATE_ACTION_HEATING;  // Indica que está calentando

  // Publica el estado a Home Assistant
  this->publish_state();
}

// Llama a update_state() en loop() o donde tiene sentido en tu lógica.


climate::ClimateTraits CustomClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);  // Soporte para mostrar la temperatura actual
  traits.set_visual_min_temperature(15.0);        // Temperatura mínima visible
  traits.set_visual_max_temperature(40.0);        // Temperatura máxima visible
  traits.set_visual_temperature_step(0.5);        // Incremento de temperatura
  traits.set_supports_two_point_target_temperature(false);  // Control de un solo punto
  traits.set_supports_action(true);               // Soporte para acciones como calentar/enfriar
  return traits;
}

void CustomClimate::check_and_control_pump() {
  if (!temperature_azotea_sensor_ || !temperature_spa_sensor_ || !temperature_caliente_sensor_ || !bomba_solar_switch_) {
    ESP_LOGW("custom_climate", "Sensores o switch no configurados");
    return;
  }

  float temperatura_azotea = temperature_azotea_sensor_->state;
  float temperatura_spa = temperature_spa_sensor_->state;
  float temperatura_caliente = temperature_caliente_sensor_->state;

  int64_t tiempo_actual = millis() / 1000;  // Convertir a segundos

  if (espera_) {
    if (tiempo_actual >= tiempo_espera_fin_) {
      espera_ = false;
      ESP_LOGI("custom_climate", "Reanudando verificaciones después de espera de 5 minutos.");
    } else {
      ESP_LOGI("custom_climate", "En espera hasta %lld", tiempo_espera_fin_);
      return;
    }
  }

  bool estado_bomba_actual = bomba_solar_switch_->state;

  if (temperatura_azotea > (temperatura_spa + temperature_difference_) && temperatura_spa < max_temperature_) {
    if (!estado_bomba_actual) {
      bomba_solar_switch_->turn_on();
      tiempo_inicio_ = tiempo_actual;
      ESP_LOGI("custom_climate", "Bomba encendida debido a la temperatura adecuada");
    }
  } else if (estado_bomba_actual) {
    bomba_solar_switch_->turn_off();
    int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
    ESP_LOGI("custom_climate", "Bomba apagada. Tiempo total de funcionamiento: %lld segundos", tiempo_total_encendido);
    espera_ = true;
    tiempo_espera_fin_ = tiempo_actual + 300;  // 5 minutos en segundos
  }

  if (estado_bomba_actual && temperatura_caliente < (temperatura_spa + 1)) {
    bomba_solar_switch_->turn_off();
    int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
    ESP_LOGI("custom_climate", "Bomba apagada debido a temperatura de salida insuficiente. Tiempo total: %lld segundos", tiempo_total_encendido);
    espera_ = true;
    tiempo_espera_fin_ = tiempo_actual + 300;
  }

  ESP_LOGD("custom_climate", "Diferencia Azotea-SPA: %.2f°C", temperatura_azotea - temperatura_spa);
  ESP_LOGD("custom_climate", "Diferencia Caliente-SPA: %.2f°C", temperatura_caliente - temperatura_spa);
  ESP_LOGD("custom_climate", "Estado de la bomba: %d", estado_bomba_actual);
}

}  // namespace custom_climate
}  // namespace esphome
