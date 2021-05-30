function sort(dict) {
  const items = Object.keys(dict).map(key => [key, dict[key]]);

  // Sort the array based on the second element
  items.sort((first, second) => second[1] - first[1]);
  const keys = [];
  const values = [];
  for (const item in items) {
    keys.push(items[item][0]);
    values.push(items[item][1]);
  }
  return {keys, values};
}

function create_pie_most_pressed(ctx, dict, qwan = 5) {
  const config = {
    type: 'pie',
    data: {
      labels: Object.keys(dict).slice(0, qwan),
      datasets: [
        {
          data: Object.values(dict).slice(0, qwan),
          backgroundColor: [
            '#FF6384',
            '#63FF84',
            '#84FF63',
            '#8463FF',
            '#6384FF'
          ]
        }]
    },
    options: {
      plugins: {
        legend: true,
        tooltip: true,
      }
    }
  };

  return new Chart(ctx, config);
}

function update_pie_most_pressed(chart, dict, qwan = 5) {
  const labels = Object.keys(dict).slice(0, qwan);
  const values = Object.values(dict).slice(0, qwan);
  chart.data.labels = labels;
  chart.data.datasets.forEach((dataset) => { dataset.data = values; });
  chart.update();
}

function create_bar_mouse_clicked(ctx, lrdata = [[0] * 5, [0] * 5], qwan = 5) {
  window.lrdata = lrdata;
  const date = new Date();
  const times = [];
  for (let i = 0; i < qwan; ++i) {
    times.unshift(`${String(date.getHours())}:${String(date.getMinutes())}:${String(date.getSeconds())}`);
    date.setTime(date - 1000);
  }

  const config = {
    type: 'bar',
    data: {
      labels: times,
      datasets: [{
        label: 'Клики левой кнопки мыши',
        backgroundColor: 'rgb(255, 99, 132)',
        borderColor: 'rgb(255, 99, 132)',
        data: lrdata[0],
        pointRadius: 10,
        pointHoverRadius: 15,
        showLine: false
      }, {
        label: 'Клики правой кнопки мыши',
        backgroundColor: 'rgb(255, 255, 132)',
        borderColor: 'rgb(255, 255, 132)',
        data: lrdata[1],
        pointRadius: 10,
        pointHoverRadius: 15,
        showLine: false
      }],
    },
    options: {
      plugins: {
        legend: true,
        tooltip: true,
      }
    }
  };

  return new Chart(ctx, config);
}

function update_bar_mouse_clicked(chart, newdata, qwan = 5) {
  const times = chart.data.labels;
  const date = new Date();
  times.push(`${String(date.getHours())}:${String(date.getMinutes())}:${String(date.getSeconds())}`);
  times = times.slice(1, qwan + 1);
  chart.data.labels = times;
  window.lrdata = newdata;
  Object.keys(chart.data.datasets[0]).forEach((dataset) => { dataset.data = [1, 2, 3]; });
  chart.update();
}