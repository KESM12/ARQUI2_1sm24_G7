<script setup>
import { ref, defineModel, watch } from "vue";

const model = defineModel("value");

const title = defineModel("title");

const show = ref(false);
const timeOut = ref(null);

watch(model, (value) => {
  if (value != "") {
    show.value = true;
    clearTimeout(timeOut.value);
    timeOut.value = setTimeout(() => {
      show.value = false;
      model.value = "";
    }, 5000);
  }
});
</script>

<template>
  <div v-if="show" class="absolute top-7 right-7">
    <div>
      <div class="bg-zinc-800 p-4 rounded-md shadow-md cursor-pointer">
        <div class="flex justify-between gap-4 items-start">
          <div>
            <p class="text-gray-300 font-semibold">{{ title }}</p>
            <p class="text-gray-300">{{ model }}</p>
          </div>
          <button @click="show = false" class="text-gray-300">&#10006;</button>
        </div>
      </div>
    </div>
  </div>
</template>
