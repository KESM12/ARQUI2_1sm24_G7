<script setup>
import { ref, defineModel } from "vue";
defineProps({
  title: String,
});
const model = defineModel({
  prop: "value",
  event: "update",
});
</script>

<template>
  <div class="mb-1">
    <div>
      <label class="text-white" for="temperature">{{ title }}</label>
    </div>
    <input
      v-model="model"
      type="text"
      class="border border-white p-1 outline-none bg-transparent text-sm text-white"
      :placeholder="title"
    />
  </div>
</template>
