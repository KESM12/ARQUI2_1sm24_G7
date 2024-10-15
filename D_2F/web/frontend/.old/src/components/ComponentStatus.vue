<script setup>
defineProps({
  icon: Array,
  title: String,
  enable: Boolean,
  value: String,
  size: String
});
defineEmits(["update"]);
</script>

<template>
  <div class="">
    <div class="text-center mb-4">
      <h2 class="text-xl font-bold text-white">{{ title }}</h2>
    </div>
    <div class="flex justify-center">
      <font-awesome-icon
        class="w-24 h-24"
        :class="enable ? 'text-green-300' : 'text-white'"
        :icon="icon"
      />
    </div>
    <div class="text-center my-4">
      <h2 :class="`${size} text-4xl font-bold text-white`">{{ value }}</h2>
    </div>
  </div>
</template>

<style scoped></style>
