
$(document).ready(function() {

  let webChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
      let interface = channel.objects.webinterface;

			$('#select-exp-button').click(e => {
				interface.openFileDialog(filepath => {
					$('#filepath-exp').val(filepath);
				});
			});

      $('#load-exp-button').click(e => {
				let filepath = $('#filepath-exp').val();
				if (filepath) {
					interface.experimentFromFile(filepath);
				}
      });

			interface.percentUpdateSignal.connect(percent => {
				$('#load-exp-progress-val').width(Math.round(100 * percent) + "%");
			});

			interface.fileErrorSignal.connect(err => {
				console.error(err);
			});
	});
});
