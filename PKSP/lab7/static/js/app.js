document.getElementById('loadJson').addEventListener('click', async () => {
  try {
    const res = await fetch('/data/data.json');
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const data = await res.json();
    document.getElementById('jsonOut').textContent = JSON.stringify(data, null, 2);
  } catch (err) {
    document.getElementById('jsonOut').textContent = 'Error: ' + err;
  }
});

document.getElementById('loadXml').addEventListener('click', async () => {
  try {
    const res = await fetch('/data/data.xml');
    if (!res.ok) throw new Error('HTTP ' + res.status);
    const text = await res.text();
    document.getElementById('xmlOut').textContent = text;
  } catch (err) {
    document.getElementById('xmlOut').textContent = 'Error: ' + err;
  }
});

