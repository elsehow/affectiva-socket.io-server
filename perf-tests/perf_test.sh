if [ $# -eq 2 ]
	then
		for video in $1/*;
		do
			TIMEFORMAT="%R";
			elapsed_time=$({ time ./emotions.exe $video >/dev/null 2>&1;} 2>&1 )
			echo $elapsed_time$'\r' >> $2
			echo "Done with $video video"
		done
else
	echo "No folder and output file passed in."
fi