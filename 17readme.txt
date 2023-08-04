// Firefox插件代码
browser.runtime.onInstalled.addListener(function() {
  // 注册表单提交事件
  browser.webRequest.onBeforeRequest.addListener(
    function(details) {
      // 获取表单数据
      var formData = details.requestBody.formData;
      
      // 保存表单数据
      browser.storage.local.set({ 'formData': formData });
    },
    { urls: ["<all_urls>"], types: ["main_frame"] },
    ["requestBody"]
  );
});

// 获取保存的表单数据
browser.storage.local.get('formData').then(function(result) {
  var formData = result.formData;
  
  // 自动填充表单数据
  if (formData) {
    Object.keys(formData).forEach(function(key) {
      var input = document.querySelector('input[name="' + key + '"]');
      if (input) {
        input.value = formData[key];
      }
    });
  }
});

// 谷歌插件代码
chrome.runtime.onInstalled.addListener(function() {
  // 注册表单提交事件
  chrome.webRequest.onBeforeRequest.addListener(
    function(details) {
      // 获取表单数据
      var formData = details.requestBody.formData;
      
      // 保存表单数据
      chrome.storage.local.set({ 'formData': formData });
    },
    { urls: ["<all_urls>"], types: ["main_frame"] },
    ["requestBody"]
  );
});

// 获取保存的表单数据
chrome.storage.local.get('formData', function(result) {
  var formData = result.formData;
  
  // 自动填充表单数据
  if (formData) {
    Object.keys(formData).forEach(function(key) {
      var input = document.querySelector('input[name="' + key + '"]');
      if (input) {
        input.value = formData[key];
      }
    });
  }
});


