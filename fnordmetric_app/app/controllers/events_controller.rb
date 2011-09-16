class EventsController < ApplicationController


	def create
		raise "malformed request" unless params[:e]
		@event = Event.track!(params[:e])
		render :text => @event.inspect
	end

end